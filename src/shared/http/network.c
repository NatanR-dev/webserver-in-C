#include "network.h"
#include <string.h>

// Platform includes
#include "../platform/platform.h"

// Platform-specific includes
#ifdef PLATFORM_WINDOWS
    #include <iphlpapi.h>
    #include <ws2tcpip.h>
#else
    #include <ifaddrs.h>
    #include <netdb.h>
    #include <net/if.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
#endif

int getLocalIP(char* ip, size_t ipSize) {
    #ifdef PLATFORM_WINDOWS
        PIP_ADAPTER_ADDRESSES pAddresses = NULL;
        ULONG outBufLen = 0;
        DWORD dwRetVal = 0;
        
        GetAdaptersAddresses(AF_INET, 0, NULL, NULL, &outBufLen);
        pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
        
        if (pAddresses == NULL) {
            return 1;
        }
        
        dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &outBufLen);
        
        if (dwRetVal == NO_ERROR) {
            PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
            
            while (pCurrAddresses) {
                if (pCurrAddresses->OperStatus == IfOperStatusUp && 
                    (pCurrAddresses->IfType == IF_TYPE_ETHERNET_CSMACD || 
                     pCurrAddresses->IfType == IF_TYPE_IEEE80211)) {  // Ethernet or WiFi
                    
                    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
                    while (pUnicast) {
                        if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                            struct sockaddr_in *sa_in = (struct sockaddr_in *)pUnicast->Address.lpSockaddr;
                            const char* ipAddr = inet_ntoa(sa_in->sin_addr);
                            
                            if (strcmp(ipAddr, "0.0.0.0") != 0 && strcmp(ipAddr, "127.0.0.1") != 0) {
                                strncpy(ip, ipAddr, ipSize - 1);
                                ip[ipSize - 1] = '\0';
                                free(pAddresses);
                                return 0;
                            }
                        }
                        pUnicast = pUnicast->Next;
                    }
                }
                pCurrAddresses = pCurrAddresses->Next;
            }
        }
        
        if (pAddresses) {
            free(pAddresses);
        }
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
