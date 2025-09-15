#include "../platform.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

int platformGetLastError(void) {
    return errno;
}

int platformNetworkingInit(void) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }
    close(sock);
    return 0;
}

int platformGetErrorString(int error, char* buffer, size_t bufferSize) {
    if (buffer == NULL || bufferSize == 0) {
        return -1;
    }
    
    char* error_str = strerror_r(error, buffer, bufferSize);
    if (error_str == NULL) {
        return -1;
    }
    
    if (error_str != buffer) {
        size_t len = strlen(error_str);
        if (len >= bufferSize) {
            len = bufferSize - 1;
        }
        strncpy(buffer, error_str, len);
        buffer[len] = '\0';
    }
    
    return 0;
}

int platformGetHostname(char* buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        return -1;
    }
    
    if (gethostname(buffer, size) != 0) {
        return -1;
    }
    
    buffer[size - 1] = '\0';
    return 0;
}

int platformGenerateMachineId(char* buffer, size_t size) {
    if (size < 17) {
        return -1;
    }
    
    int fd = open("/etc/machine-id", O_RDONLY);
    if (fd != -1) {
        ssize_t bytes_read = read(fd, buffer, size - 1);
        close(fd);
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            char* newline = strchr(buffer, '\n');
            if (newline) *newline = '\0';
            return 0;
        }
    }
    
    struct utsname name;
    if (uname(&name) != 0) {
        return -1;
    }
    
    size_t len = strlen(name.nodename);
    if (len >= size) {
        len = size - 1;
    }
    
    strncpy(buffer, name.nodename, len);
    buffer[len] = '\0';
    return 0;
}

#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static int getLocalIpByConnection(char* buffer, size_t size) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return -1;
    }
    
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("8.8.8.8");
    serv.sin_port = htons(53);
    
    if (connect(sock, (struct sockaddr*)&serv, sizeof(serv)) < 0) {
        close(sock);
        return -1;
    }
    
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(sock, (struct sockaddr*)&local_addr, &addr_len) < 0) {
        close(sock);
        return -1;
    }
    
    close(sock);
    const char* ip = inet_ntoa(local_addr.sin_addr);
    if (ip == NULL) {
        return -1;
    }
    
    strncpy(buffer, ip, size - 1);
    buffer[size - 1] = '\0';
    return 0;
}

int platformGetLocalIp(char* buffer, size_t size) {
    if (size < INET6_ADDRSTRLEN) {
        return -1;
    }
    
    buffer[0] = '\0';
    int result = -1;
    
    if (getLocalIpByConnection(buffer, size) == 0) {
        return 0;
    }
    
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == -1) {
        return -1;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        
        if (ifa->ifa_addr->sa_family == AF_INET) {
            if ((ifa->ifa_flags & IFF_LOOPBACK) || !(ifa->ifa_flags & IFF_UP)) {
                continue;
            }
            
            struct sockaddr_in* sa = (struct sockaddr_in*) ifa->ifa_addr;
            const char* ip = inet_ntoa(sa->sin_addr);
            
            if (ip && strcmp(ip, "127.0.0.1") != 0 && 
                strncmp(ip, "169.254.", 8) != 0) {
                
                strncpy(buffer, ip, size - 1);
                buffer[size - 1] = '\0';
                result = 0;
                break;
            }
        }
    }
    
    if (result != 0) {
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            struct addrinfo hints = {0}, *addrs;
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            
            if (getaddrinfo(hostname, NULL, &hints, &addrs) == 0) {
                for (struct addrinfo* addr = addrs; addr != NULL; addr = addr->ai_next) {
                    if (addr->ai_family == AF_INET) {
                        struct sockaddr_in* ipv4 = (struct sockaddr_in*)addr->ai_addr;
                        const char* ip = inet_ntoa(ipv4->sin_addr);
                        if (ip && strcmp(ip, "127.0.0.1") != 0) {
                            strncpy(buffer, ip, size - 1);
                            buffer[size - 1] = '\0';
                            result = 0;
                            break;
                        }
                    }
                }
                freeaddrinfo(addrs);
            }
        }
    }
    
    if (result != 0) {
        strncpy(buffer, "127.0.0.1", size - 1);
        buffer[size - 1] = '\0';
        result = 0;
    }
    
    freeifaddrs(ifaddr);
    return result;
}
