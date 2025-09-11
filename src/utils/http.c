#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    // Windows
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
#else
    // Unix-like
    #include <unistd.h>
    #include <ifaddrs.h>
    #include <netdb.h>
    #include <net/if.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <sys/types.h>
#endif

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

void sendErrorResponse(int clientConnection, int statusCode, 
        const char* statusMessage, const char* body) {
    sendHttpResponse(clientConnection, statusCode, statusMessage, 
        "text/plain", body, "close");
}

int getLocalIP(char* ip, size_t ipSize) {
    #ifdef _WIN32
        // Windows 
        PIP_ADAPTER_INFO pAdapterInfo;
        PIP_ADAPTER_INFO pAdapter = NULL;
        DWORD dwRetVal = 0;
        ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
        
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
        if (pAdapterInfo == NULL) {
            return 1;
        }
        
        if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
            free(pAdapterInfo);
            pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
            if (pAdapterInfo == NULL) {
                return 1;
            }
        }

        if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
            pAdapter = pAdapterInfo;
            while (pAdapter) {
                if (pAdapter->Type == MIB_IF_TYPE_ETHERNET || 
                    pAdapter->Type == IF_TYPE_IEEE80211) {  // Ethernet or WiFi
                    IP_ADDR_STRING *pIpAddr = &(pAdapter->IpAddressList);
                    while (pIpAddr) {
                        if (strcmp(pIpAddr->IpAddress.String, "0.0.0.0") != 0 &&
                            strcmp(pIpAddr->IpAddress.String, "127.0.0.1") != 0) {
                            strncpy_s(ip, ipSize, pIpAddr->IpAddress.String, _TRUNCATE);
                            free(pAdapterInfo);
                            return 0;
                        }
                        pIpAddr = pIpAddr->Next;
                    }
                }
                pAdapter = pAdapter->Next;
            }
        }
        
        free(pAdapterInfo);
        return 1;
    #else
        // Unix-like
        struct ifaddrs *ifaddr, *ifa;
        int family, s;
        char host[NI_MAXHOST];

        if (getifaddrs(&ifaddr) == -1) {
            return 1;
        }

        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL) {
                continue;
            }

            family = ifa->ifa_addr->sa_family;

            if (family == AF_INET) {  // IPv4
                s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                    host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
                if (s != 0) {
                    continue;
                }

                if (strcmp(host, "127.0.0.1") == 0 || 
                    !(ifa->ifa_flags & IFF_UP) || 
                    (ifa->ifa_flags & IFF_LOOPBACK)) {
                    continue;
                }

                strncpy(ip, host, ipSize - 1);
                ip[ipSize - 1] = '\0';
                freeifaddrs(ifaddr);
                return 0;
            }
        }

        freeifaddrs(ifaddr);
        return 1;
    #endif
}
