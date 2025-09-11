// Common includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Platform includes
#include "../shared/platform/platform.h"

// Imports
#include "server.h"
#include "../routes/routes.h"

// Constants
#define PORT 8080
#define BUFFER_SIZE 4096

int handleRequest(Server* server, PLATFORM_SOCKET clientConnection, char* request);

void handleClient(Server* server, PLATFORM_SOCKET clientConnection);

void initSockets() {
    platformNetworkingInit();
}

void cleanupSockets() {
    platformNetworkingCleanup();
}

void closeConnection(PLATFORM_SOCKET clientConnection, char* response) {
    if (response && *response) {
        send(clientConnection, response, (int)strlen(response), 0);
    }
    shutdown(clientConnection, PLATFORM_SHUT_RDWR);
    PLATFORM_CLOSE_SOCKET(clientConnection);
}

void startServer(Server* server, int port) {
    if (!server) {
        return;
    }
    
    server->socket = -1;
    server->port = port;  
    server->routeCount = 0;
    
    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket < 0) {
        perror("Socket creation failed");
        return;
    }
    
    int opt = 1;
    if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        PLATFORM_CLOSE_SOCKET(server->socket);
        server->socket = -1;
        return;
    }
    
    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);
    
    if (bind(server->socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Bind failed");
        PLATFORM_CLOSE_SOCKET(server->socket);
        server->socket = -1;
        return;
    }
    
    if (listen(server->socket, 5) < 0) {
        perror("Listen failed");
        PLATFORM_CLOSE_SOCKET(server->socket);
        server->socket = -1;
        return;
    }
    
}

void serverListening(Server* server, void (*clientHandler)(Server*, PLATFORM_SOCKET)) {
    if (!server || server->socket < 0 || !clientHandler) {
        return;
    }
    
    printf("Server started on port %d, waiting for connections...\n", server->port);
    
    while (1) {
        struct sockaddr_in clientAddress = {0};
        socklen_t clientSize = sizeof(clientAddress);
        PLATFORM_SOCKET clientConnection = accept(server->socket, (struct sockaddr*)&clientAddress, &clientSize);
        if (clientConnection == INVALID_PLATFORM_SOCKET) {
            perror("Accept failed");
            continue;
        }
        
        printf("Connection received from %s\n", inet_ntoa(clientAddress.sin_addr));
        clientHandler(server, clientConnection);
    }
}

void cleanupServer(Server* server) {
    if (!server) {
        return;
    }
    
    if (server->socket >= 0) {
        PLATFORM_CLOSE_SOCKET(server->socket);
        server->socket = -1;
    }
    
    for (int i = 0; i < server->routeCount; i++) {
        if (server->routes[i].path) {
            free(server->routes[i].path);
            server->routes[i].path = NULL;
        }
    }
}
