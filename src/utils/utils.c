#include "utils.h"

// Common
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    // WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
#else
    // UNIX-LIKE
    #include <unistd.h>
    #include <ifaddrs.h>
    #include <netdb.h>
    #include <net/if.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <sys/types.h>
#endif

void jsonEscapeString(const char* input, char* output, size_t outputSize) {
    size_t i = 0;
    size_t j = 0;
    while (input[i] != '\0' && j < outputSize - 1) {
        switch (input[i]) {
            case '\\':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = '\\';
                break;
            case '"':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = '"';
                break;
            case '\b':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = 'b';
                break;
            case '\f':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = 'f';
                break;
            case '\n':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = 'n';
                break;
            case '\r':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = 'r';
                break;
            case '\t':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = 't';
                break;
            default:
                if (input[i] >= ' ' && input[i] <= '~') {
                    output[j++] = input[i];
                } else {
                    snprintf(output + j, outputSize - j, "\\u%04x", input[i]);
                    j += 6;
                }
                break;
        }
        i++;
    }
    output[j] = '\0';
}

void sendHttpResponse(int clientConnection, int statusCode, const char* statusMessage, 
                     const char* contentType, const char* body, const char* connection) {
    int requiredSize = snprintf(NULL, 0, 
        "HTTP/1.1 %d %s\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: %s\r\n"
        "\r\n"
        "%s", 
        statusCode, statusMessage, contentType, (int)strlen(body), connection, body) + 1;
        
    char* response = (char*)malloc(requiredSize);
    if (!response) return;
    
    snprintf(response, requiredSize,
        "HTTP/1.1 %d %s\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: %s\r\n"
        "\r\n"
        "%s", 
        statusCode, statusMessage, contentType, (int)strlen(body), connection, body);
        
    #ifdef _WIN32
    send(clientConnection, response, (int)strlen(response), 0);
    #else
    write(clientConnection, response, strlen(response));
    #endif
    
    free(response);
}

void sendJsonResponse(int clientConnection, const char* json) {
    sendHttpResponse(clientConnection, 200, "OK", "application/json", json, "keep-alive");
}

void sendErrorResponse(int clientConnection, int statusCode, 
                      const char* statusMessage, const char* body) {
    sendHttpResponse(clientConnection, statusCode, statusMessage, "text/plain", body, "close");
}

int getLocalIP(char* ip, size_t ipSize) {
    if (ip == NULL || ipSize < 16) {  
        return -1;
    }

#ifdef _WIN32
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    DWORD dwRetVal = 0;
    
    dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, NULL, &outBufLen);
    if (dwRetVal != ERROR_BUFFER_OVERFLOW) {
        return -1;
    }
    
    pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
    if (pAddresses == NULL) {
        return -1;
    }
    
    dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &outBufLen);
    
    if (dwRetVal == NO_ERROR) {
        PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            if (pCurrAddresses->OperStatus == IfOperStatusUp && 
                pCurrAddresses->IfType != IF_TYPE_SOFTWARE_LOOPBACK) {
                    
                PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
                while (pUnicast) {
                    if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                        struct sockaddr_in *ipv4 = (struct sockaddr_in *)pUnicast->Address.lpSockaddr;
                        inet_ntop(AF_INET, &(ipv4->sin_addr), ip, ipSize);
                        free(pAddresses);
                        return 0;
                    }
                    pUnicast = pUnicast->Next;
                }
            }
            pCurrAddresses = pCurrAddresses->Next;
        }
    }
    
    free(pAddresses);
    return -1;
#else
    struct ifaddrs *ifaddr = NULL;
    struct ifaddrs *ifa = NULL;
    int result = -1;
    
    if (getifaddrs(&ifaddr) == -1) {
        return -1;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        
        int family = ifa->ifa_addr->sa_family;
        
        if (family == AF_INET && 
            !(ifa->ifa_flags & IFF_LOOPBACK) && 
            (ifa->ifa_flags & IFF_UP) &&
            (ifa->ifa_flags & IFF_RUNNING)) {
                
            struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
            if (inet_ntop(AF_INET, &(addr->sin_addr), ip, ipSize) != NULL) {
                result = 0;  
                break;
            }
        }
    }
    
    freeifaddrs(ifaddr);
    return result;
#endif
}

void generateMachineId(char* id, size_t idSize) {
    if (id == NULL || idSize < 9) {  
        if (id != NULL && idSize > 0) {
            id[0] = '\0';  
        }
        return;
    }

    char hostname[256] = {0};
    int hostnameResult = -1;
    
#ifdef _WIN32
    DWORD size = (DWORD)sizeof(hostname);
    hostnameResult = (GetComputerNameA(hostname, &size) == 0) ? -1 : 0;
#else
    hostnameResult = gethostname(hostname, sizeof(hostname) - 1);
#endif
    
    if (hostnameResult != 0) {
        strncpy(hostname, "unknown-host", sizeof(hostname) - 1);
    }
    hostname[sizeof(hostname) - 1] = '\0';  
    
    unsigned long hash = 5381;
    int c;
    const unsigned char* str = (const unsigned char*)hostname;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; 
    }
    
    time_t now = time(NULL);
    hash = hash ^ (unsigned long)now;
    
    snprintf(id, idSize, "%08lx", hash);
    id[idSize - 1] = '\0'; 
}
