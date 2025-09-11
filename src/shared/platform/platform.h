#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0600  // For Windows Vista or later
    #endif
#else
    #define PLATFORM_UNIX 1
#endif

// Common 
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

// Platform-specific includes
#ifdef PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
    
    // Windows-specific types and macros
    #define PLATFORM_SOCKET SOCKET
    #define INVALID_PLATFORM_SOCKET INVALID_SOCKET
    #define PLATFORM_SOCKET_ERROR SOCKET_ERROR
    #define PLATFORM_CLOSE_SOCKET(s) closesocket(s)
    #define PLATFORM_SHUT_RDWR SD_BOTH
    #define PLATFORM_STARTUP() \
        do { \
            WSADATA wsaData; \
            WSAStartup(MAKEWORD(2, 2), &wsaData); \
        } while(0)
    #define PLATFORM_CLEANUP() WSACleanup()
    
    #ifdef _MSC_VER
        #pragma comment(lib, "ws2_32.lib")
        #pragma comment(lib, "iphlpapi.lib")
    #endif
#else
    // UNIX/Linux includes
    #include <unistd.h>
    #include <ifaddrs.h>
    #include <netdb.h>
    #include <net/if.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    
    // UNIX-specific types and macros
    #define PLATFORM_SOCKET int
    #define INVALID_PLATFORM_SOCKET (-1)
    #define PLATFORM_SOCKET_ERROR (-1)
    #define PLATFORM_CLOSE_SOCKET(s) close(s)
    #define PLATFORM_SHUT_RDWR SHUT_RDWR
    #define PLATFORM_STARTUP()
    #define PLATFORM_CLEANUP()
#endif

// Platform-independent API

/**
 * @brief Get the last network error code
 * @return Platform-specific error code
 */
int platformGetLastError(void);

/**
 * @brief Get a human-readable error message for the given error code
 * @param error Error code (from platformGetLastError())
 * @param buffer Buffer to store the error message
 * @param bufferSize Size of the buffer
 * @return 0 on success, non-zero on failure
 */
int platformGetErrorString(int error, char* buffer, size_t bufferSize);

/**
 * @brief Initialize platform-specific networking
 * @return 0 on success, non-zero on failure
 */
int platformNetworkingInit(void);

/**
 * @brief Clean up platform-specific networking resources
 */
void platformNetworkingCleanup(void);

/**
 * @brief Get the local machine's hostname
 * @param buffer Buffer to store the hostname
 * @param size Size of the buffer
 * @return 0 on success, non-zero on failure
 */
int platformGetHostname(char* buffer, size_t size);

#endif 
