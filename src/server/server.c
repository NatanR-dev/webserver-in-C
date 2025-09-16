// Common includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Platform includes
#include "../shared/platform/platform.h"

// Shared HTTP includes
#include "../shared/http/response.h"
#include "../shared/http/network.h"

// Server includes
#include "server.h"
#include "../shared/router/router.h"

// Constants
#define PORT 8080
#define BUFFER_SIZE 4096


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
    if (server == NULL || request == NULL) {
        sendErrorResponse(clientConnection, 400, "Bad Request", "Invalid request");
        return;
    }
    
    if (clientConnection == INVALID_PLATFORM_SOCKET) {
        return;
    }
    
    if (strncmp(request, "GET ", 4) != 0) {
        sendErrorResponse(clientConnection, 405, "Method Not Allowed", "Only GET method is supported");
        return;
    }
    
    const char* path = request + 4;
    const char* pathEnd = strchr(path, ' ');
    if (pathEnd == NULL) {
        sendErrorResponse(clientConnection, 400, "Bad Request", "Invalid request format");
        return;
    }
    
    size_t pathLen = (size_t)(pathEnd - path);
    if (pathLen == 0 || pathLen >= 256) {
        sendErrorResponse(clientConnection, 400, "Bad Request", "Invalid path length");
        return;
    }
    
    char pathCopy[256] = {0};
    strncpy(pathCopy, path, pathLen);
    pathCopy[pathLen] = '\0';
    
    for (int i = 0; i < server->routeCount; i++) {
        if (server->routes[i].path != NULL && strcmp(server->routes[i].path, pathCopy) == 0) {
            server->routes[i].handler(server, clientConnection);
            return;
        }
    }
    
    char response[256];
    snprintf(response, sizeof(response), "{\"error\":\"Not Found\",\"path\":\"%s\"}", pathCopy);
    sendJsonResponse(clientConnection, response);
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
