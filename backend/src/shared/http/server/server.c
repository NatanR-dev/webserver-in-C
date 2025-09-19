// Common 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Platform
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
    #define PLATFORM_SHUT_RDWR SD_BOTH
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <fcntl.h>
    #define PLATFORM_SHUT_RDWR SHUT_RDWR
#endif

// Shared HTTP 
#include "../network/network.h"

// Server 
#include "server.h"
#include "../router/router.h"
#include "../response/response.h"

// Constants
#define PORT 8080

static const char* httpMethodToString(HttpMethod method) {
    switch (method) {
        case HTTP_GET:     return "GET";
        case HTTP_POST:    return "POST";
        case HTTP_PUT:     return "PUT";
        case HTTP_DEL:     return "DELETE";
        case HTTP_PATCH:   return "PATCH";
        case HTTP_OPTIONS: return "OPTIONS";
        case HTTP_HEAD:    return "HEAD";
        default:           return "UNKNOWN";
    }
}

void initSockets(void) {
    if (platformNetworkingInit() != 0) {
        fprintf(stderr, "Failed to initialize networking\n");
        exit(EXIT_FAILURE);
    }
}

void cleanupSockets(void) {
    platformNetworkingCleanup();
}

int initializeServer(Server* server, int port) {
    if (server == NULL) {
        return -1;
    }
    
    memset(server, 0, sizeof(Server));
    server->port = port;
    server->routeCount = 0;
    server->socket = INVALID_PLATFORM_SOCKET;
    server->requestBody = NULL;
    
    server->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server->socket == INVALID_PLATFORM_SOCKET) {
        char errorMsg[256];
        platformGetErrorString(platformGetLastError(), errorMsg, sizeof(errorMsg));
        fprintf(stderr, "Failed to create socket: %s\n", errorMsg);
        return -1;
    }
    int opt = 1;
    if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) == PLATFORM_SOCKET_ERROR) {
        char errorMsg[256];
        platformGetErrorString(platformGetLastError(), errorMsg, sizeof(errorMsg));
        fprintf(stderr, "Failed to set socket options: %s\n", errorMsg);
        PLATFORM_CLOSE_SOCKET(server->socket);
        server->socket = INVALID_PLATFORM_SOCKET;
        return -1;
    }
    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons((uint16_t)port);
    
    if (bind(server->socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == PLATFORM_SOCKET_ERROR) {
        char errorMsg[256];
        platformGetErrorString(platformGetLastError(), errorMsg, sizeof(errorMsg));
        fprintf(stderr, "Failed to bind socket: %s\n", errorMsg);
        PLATFORM_CLOSE_SOCKET(server->socket);
        server->socket = INVALID_PLATFORM_SOCKET;
        return -1;
    }
    
    if (listen(server->socket, SOMAXCONN) == PLATFORM_SOCKET_ERROR) {
        char errorMsg[256];
        platformGetErrorString(platformGetLastError(), errorMsg, sizeof(errorMsg));
        fprintf(stderr, "Failed to listen on socket: %s\n", errorMsg);
        PLATFORM_CLOSE_SOCKET(server->socket);
        server->socket = INVALID_PLATFORM_SOCKET;
        return -1;
    }
    
    return 0;
}

void cleanupServer(Server* server) {
    if (server == NULL) {
        return;
    }
    
    if (server->socket != INVALID_PLATFORM_SOCKET) {
        PLATFORM_CLOSE_SOCKET(server->socket);
        server->socket = INVALID_PLATFORM_SOCKET;
    }
    
    for (int i = 0; i < server->routeCount; i++) {
        if (server->routes[i].path != NULL) {
            free(server->routes[i].path);
            server->routes[i].path = NULL;
        }
    }
    
    server->routeCount = 0;
}

void closeConnection(PLATFORM_SOCKET clientConnection, const char* message) {
    if (clientConnection == INVALID_PLATFORM_SOCKET) {
        return;
    }
    
    if (message != NULL && *message != '\0') {
        send(clientConnection, message, (int)strlen(message), 0);
    }
    
    shutdown(clientConnection, PLATFORM_SHUT_RDWR);
    PLATFORM_CLOSE_SOCKET(clientConnection);
}


void handleRequest(Server* server, PLATFORM_SOCKET clientConnection, const char* request) {
    printf("LOG: Handling new request\n");
    
    if (server == NULL || request == NULL) {
        printf("ERROR: Invalid parameters to handleRequest\n");
        closeConnection(clientConnection, "Invalid request");
        return;
    }
    
    printf("LOG: Request: %.*s...\n", 100, request);
    
    char method[16] = {0};
    char path[256] = {0};
    char* body = NULL;
    
    if (sscanf(request, "%15s %255s", method, path) != 2) {
        sendErrorResponse(clientConnection, 400, "Bad Request", "Invalid request format");
        closeConnection(clientConnection, NULL);
        return;
    }
    
    char* headerEnd = strstr(request, "\r\n\r\n");
    if (headerEnd) {
        body = headerEnd + 4; 
        
        if (strcmp(method, "POST") == 0 || strcmp(method, "PUT") == 0) {
            char* contentLengthHeader = strstr(request, "Content-Length: ");
            if (contentLengthHeader) {
                int contentLength = atoi(contentLengthHeader + 16); 
                size_t bodyLength = strlen(body);
                
                if (bodyLength < (size_t)contentLength) {
                    body[bodyLength] = '\0';
                }
            }
        }
    }
    
    int requestMethod = HTTP_GET; 
    if (strcmp(method, "POST") == 0) requestMethod = HTTP_POST;
    else if (strcmp(method, "PUT") == 0) requestMethod = HTTP_PUT;
    else if (strcmp(method, "DELETE") == 0) requestMethod = HTTP_DEL; 
    else if (strcmp(method, "PATCH") == 0) requestMethod = HTTP_PATCH;
    else if (strcmp(method, "OPTIONS") == 0) requestMethod = HTTP_OPTIONS;
    else if (strcmp(method, "HEAD") == 0) requestMethod = HTTP_HEAD;
    
    if ((requestMethod == HTTP_POST || requestMethod == HTTP_PUT) && body) {
        server->requestBody = body;
    }
    
    printf("LOG: Looking for route: %s %s\n", method, path);
    
    for (int i = 0; i < server->routeCount; i++) {
        printf("LOG: Checking route %d: %s %s\n", i, 
               httpMethodToString(server->routes[i].method), 
               server->routes[i].path);
               
        if (strcmp(server->routes[i].path, path) == 0 && 
            server->routes[i].method == (HttpMethod)requestMethod) {
            printf("LOG: Found matching route, calling handler\n");
            server->routes[i].handler((void*)server, (void*)(intptr_t)clientConnection);
            server->requestBody = NULL; 
            printf("LOG: Handler completed\n");
            return;
        }
    }
    
    printf("LOG: No matching route found\n");
    
    bool pathExists = false;
    for (int i = 0; i < server->routeCount; i++) {
        if (strcmp(server->routes[i].path, path) == 0) {
            pathExists = true;
            break;
        }
    }
    
    if (pathExists) {
        sendErrorResponse(clientConnection, 405, "Method Not Allowed", 
                         "The requested method is not allowed for this resource");
    } else {
        httpNotFound(clientConnection, path);
    }
    
    closeConnection(clientConnection, NULL);
}

void sendResponse(PLATFORM_SOCKET clientConnection, const char* response) {
    if (clientConnection == INVALID_PLATFORM_SOCKET || response == NULL) {
        return;
    }
    
    size_t len = strlen(response);
    if (len > 0) {
        send(clientConnection, response, (int)len, 0);
    }
}

void serverListening(Server* server, void (*clientHandler)(Server*, PLATFORM_SOCKET)) {
    if (server == NULL || clientHandler == NULL || server->socket == INVALID_PLATFORM_SOCKET) {
        fprintf(stderr, "Invalid server parameters in serverListening\n");
        return;
    }
    
    printf("Server listening on port %d...\n", server->port);
    
    while (1) {
        PLATFORM_SOCKET clientSocket = acceptConnection(server);
        if (clientSocket != INVALID_PLATFORM_SOCKET) {
            clientHandler(server, clientSocket);
        } else {
            #ifdef _WIN32
                Sleep(100);
            #else
                usleep(100000);
            #endif
        }
    }
}

PLATFORM_SOCKET acceptConnection(Server* server) {
    if (server == NULL || server->socket == INVALID_PLATFORM_SOCKET) {
        return INVALID_PLATFORM_SOCKET;
    }
    
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    PLATFORM_SOCKET clientSocket = accept(server->socket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    
    if (clientSocket == INVALID_PLATFORM_SOCKET) {
        char errorMsg[256];
        platformGetErrorString(platformGetLastError(), errorMsg, sizeof(errorMsg));
        fprintf(stderr, "Failed to accept connection: %s\n", errorMsg);
        return INVALID_PLATFORM_SOCKET;
    }
    
    printf("Connection accepted from %s\n", inet_ntoa(clientAddr.sin_addr));
    return clientSocket;
}
