#include "../platform.h"
#include <stdio.h>
#include <rpc.h>
#include <iphlpapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <windows.h>

#ifdef _MSC_VER
    #pragma comment(lib, "iphlpapi.lib")
    #pragma comment(lib, "ws2_32.lib")
#endif

int platformGetLastError(void) {
    return WSAGetLastError();
}

int platformGetErrorString(int error, char* buffer, size_t bufferSize) {
    if (!buffer || bufferSize == 0) {
        return -1;
    }
    
    DWORD result = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        (DWORD)error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)buffer,
        (DWORD)bufferSize,
        NULL);
        
    return (result > 0) ? 0 : -1;
}

int platformNetworkingInit(void) {
    static int initialized = 0;
    
    if (initialized) {
        return 0;
    }
    
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        return -1;
    }
    
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        return -1;
    }
    
    (void)RpcStringFreeA(NULL); 
    
    initialized = 1;
    return 0;
}

void platformNetworkingCleanup(void) {

    (void)RpcMgmtStopServerListening(NULL);
    
    WSACleanup();
}

int platformGetHostname(char* buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        return -1;
    }
    
    DWORD bufferSize = (DWORD)size;
    if (GetComputerNameA(buffer, &bufferSize) == 0) {
        return -1;
    }
    
    return 0;
}

#include <stdio.h>  

int platformGenerateMachineId(char* buffer, size_t size) {
    if (size < 9) {
        return -1;
    }
    
    buffer[0] = '\0';
    
    UUID uuid;
    RPC_CSTR uuidStr = NULL;
    int result = -1;
    
    if (UuidCreate(&uuid) == RPC_S_OK) {
        if (UuidToStringA(&uuid, &uuidStr) == RPC_S_OK) {
            strncpy_s(buffer, 9, (char*)uuidStr, 8);
            buffer[8] = '\0';
            result = 0;
            RpcStringFreeA(&uuidStr);
        }
    }
    
    if (result != 0) {
        DWORD nameSize = (DWORD)size;
        if (GetComputerNameA(buffer, &nameSize)) {
            result = 0;
        }
    }
    
    if (result != 0) {
        time_t now = time(NULL);
        _snprintf_s(buffer, size, _TRUNCATE, "id_%llu", (unsigned long long)now);
        result = 0;
    }
    
    return result;
}

static int getLocalIpByConnection(char* buffer, size_t size) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        return -1;
    }
    
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("8.8.8.8");
    serv.sin_port = htons(53);  
    
    if (connect(sock, (struct sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR) {
        closesocket(sock);
        return -1;
    }
    
    struct sockaddr_in local_addr;
    int addr_len = sizeof(local_addr);
    if (getsockname(sock, (struct sockaddr*)&local_addr, &addr_len) == SOCKET_ERROR) {
        closesocket(sock);
        return -1;
    }
    
    closesocket(sock);
    const char* ip = inet_ntoa(local_addr.sin_addr);
    if (ip == NULL) {
        return -1;
    }
    
    strncpy_s(buffer, size, ip, _TRUNCATE);
    return 0;
}

int platformGetLocalIp(char* buffer, size_t size) {
    if (size < INET6_ADDRSTRLEN) {
        return -1;
    }
    
    buffer[0] = '\0';
    
    if (getLocalIpByConnection(buffer, size) == 0) {
        return 0;
    }
    
    ULONG outBufLen = 15000; 
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG family = AF_UNSPEC;
    ULONG retVal = 0;
    int result = -1;

    pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
    if (pAddresses == NULL) {
        return -1;
    }

    retVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
    if (retVal == ERROR_BUFFER_OVERFLOW) {
        free(pAddresses);
        pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
        if (pAddresses == NULL) {
            return -1;
        }
    } else if (retVal != ERROR_SUCCESS) {
        free(pAddresses);
        return -1;
    }

    retVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
    if (retVal == ERROR_SUCCESS) {
        PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
        
        while (pCurrAddresses) {
            if (pCurrAddresses->IfType != IF_TYPE_SOFTWARE_LOOPBACK && 
                pCurrAddresses->OperStatus == IfOperStatusUp) {
                
                PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
                
                while (pUnicast) {
                    if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                        struct sockaddr_in* sa_in = (struct sockaddr_in*)pUnicast->Address.lpSockaddr;
                        const char* ip = inet_ntoa(sa_in->sin_addr);
                        
                        if (ip && strcmp(ip, "127.0.0.1") != 0 && 
                            strncmp(ip, "169.254.", 8) != 0) {
                            
                            strncpy_s(buffer, size, ip, _TRUNCATE);
                            result = 0;
                            break;
                        }
                    }
                    
                    if (result == 0) break; 
                    pUnicast = pUnicast->Next;
                }
            }
            
            if (result == 0) break; 
            pCurrAddresses = pCurrAddresses->Next;
        }
    }
    
    if (result != 0) {
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            struct hostent* host = gethostbyname(hostname);
            if (host != NULL) {
                for (int i = 0; host->h_addr_list[i] != NULL; i++) {
                    struct in_addr addr;
                    memcpy(&addr, host->h_addr_list[i], sizeof(struct in_addr));
                    const char* ip = inet_ntoa(addr);
                    
                    if (ip && strcmp(ip, "127.0.0.1") != 0 && 
                        strncmp(ip, "169.254.", 8) != 0) {
                        
                        strncpy_s(buffer, size, ip, _TRUNCATE);
                        result = 0;
                        break;
                    }
                }
            }
        }
    }
    
    if (result != 0) {
        strncpy_s(buffer, size, "127.0.0.1", _TRUNCATE);
        result = 0;
    }
    
    free(pAddresses);
    return result;
}
