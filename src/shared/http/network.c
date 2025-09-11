#include "network.h"
#include <string.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <iphlpapi.h>
    #include <ws2tcpip.h>
#else
    #include <ifaddrs.h>
    #include <netdb.h>
    #include <net/if.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <netinet/in.h>
#endif

int getLocalIP(char* ip, size_t ipSize) {
    #ifdef _WIN32
        // Windows implementation
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
                            #ifdef _MSC_VER
                                strncpy_s(ip, ipSize, pIpAddr->IpAddress.String, _TRUNCATE);
                            #else
                                strncpy(ip, pIpAddr->IpAddress.String, ipSize - 1);
                                ip[ipSize - 1] = '\0';
                            #endif
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
        // Unix-like implementation
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
