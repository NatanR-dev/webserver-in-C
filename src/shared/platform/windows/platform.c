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
    
    // Only initialize once
    if (initialized) {
        return 0;
    }
    
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        return -1;
    }
    
    // Initialize Winsock
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        return -1;
    }
    
    // Initialize RPC
    (void)RpcStringFreeA(NULL); // Just to initialize RPC
    
    initialized = 1;
    return 0;
}

void platformNetworkingCleanup(void) {
    // Clean up RPC
    (void)RpcMgmtStopServerListening(NULL);
    
    // Clean up Winsock
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

#include <stdio.h>  // Add this at the top of the file if not already present

int platformGenerateMachineId(char* buffer, size_t size) {
    if (size < 37) { // 36 chars for UUID + null terminator
        return -1;
    }
    
    // Initialize buffer to empty string
    buffer[0] = '\0';
    
    // First try: Get UUID
    UUID uuid;
    RPC_CSTR uuidStr = NULL;
    int result = -1;
    
    if (UuidCreate(&uuid) == RPC_S_OK) {
        if (UuidToStringA(&uuid, &uuidStr) == RPC_S_OK) {
            // Ensure we don't exceed the buffer size
            size_t len = strlen((char*)uuidStr);
            if (len < size) {
                strncpy_s(buffer, size, (char*)uuidStr, _TRUNCATE);
                result = 0;
            }
            RpcStringFreeA(&uuidStr);
        }
    }
    
    // Second try: If UUID generation failed, fall back to computer name
    if (result != 0) {
        DWORD nameSize = (DWORD)size;
        if (GetComputerNameA(buffer, &nameSize)) {
            result = 0;
        }
    }
    
    // Final fallback: Use a timestamp if everything else fails
    if (result != 0) {
        time_t now = time(NULL);
        _snprintf_s(buffer, size, _TRUNCATE, "id_%llu", (unsigned long long)now);
        result = 0;
    }
    
    return result;
}

// Helper function to get local IP by connecting to an external server
static int getLocalIpByConnection(char* buffer, size_t size) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        return -1;
    }
    
    // Connect to Google's public DNS server (doesn't actually send any data)
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("8.8.8.8");
    serv.sin_port = htons(53);  // DNS port
    
    if (connect(sock, (struct sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR) {
        closesocket(sock);
        return -1;
    }
    
    // Get the local address that was used for the connection
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
    
    // Initialize the buffer
    buffer[0] = '\0';
    
    // First try: Direct connection method (most reliable)
    if (getLocalIpByConnection(buffer, size) == 0) {
        return 0;
    }
    
    // Fall back to GetAdaptersAddresses if direct connection failed
    ULONG outBufLen = 15000; // 15KB initial buffer size
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG family = AF_UNSPEC;
    ULONG retVal = 0;
    int result = -1;

    // Allocate initial buffer
    pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
    if (pAddresses == NULL) {
        return -1;
    }

    // Make an initial call to get the needed size
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

    // Make the actual call
    retVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
    if (retVal == ERROR_SUCCESS) {
        PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
        
        // Iterate through all adapters
        while (pCurrAddresses) {
            // Skip loopback adapters and non-IPv4 interfaces
            if (pCurrAddresses->IfType != IF_TYPE_SOFTWARE_LOOPBACK && 
                pCurrAddresses->OperStatus == IfOperStatusUp) {
                
                PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
                
                // Iterate through all unicast addresses
                while (pUnicast) {
                    // Check for IPv4 address
                    if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                        struct sockaddr_in* sa_in = (struct sockaddr_in*)pUnicast->Address.lpSockaddr;
                        const char* ip = inet_ntoa(sa_in->sin_addr);
                        
                        // Skip loopback and link-local addresses
                        if (ip && strcmp(ip, "127.0.0.1") != 0 && 
                            strncmp(ip, "169.254.", 8) != 0) {
                            
                            // Copy the IP address to the buffer
                            strncpy_s(buffer, size, ip, _TRUNCATE);
                            result = 0;
                            break;
                        }
                    }
                    
                    if (result == 0) break; // Found a valid IP
                    pUnicast = pUnicast->Next;
                }
            }
            
            if (result == 0) break; // Found a valid IP
            pCurrAddresses = pCurrAddresses->Next;
        }
    }
    
    // Fallback to gethostbyname if the previous methods failed
    if (result != 0) {
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            struct hostent* host = gethostbyname(hostname);
            if (host != NULL) {
                // Get the first non-loopback IPv4 address
                for (int i = 0; host->h_addr_list[i] != NULL; i++) {
                    struct in_addr addr;
                    memcpy(&addr, host->h_addr_list[i], sizeof(struct in_addr));
                    const char* ip = inet_ntoa(addr);
                    
                    // Skip loopback and link-local addresses
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
    
    // If still no IP found, use localhost as last resort
    if (result != 0) {
        strncpy_s(buffer, size, "127.0.0.1", _TRUNCATE);
        result = 0;
    }
    
    free(pAddresses);
    return result;
}
