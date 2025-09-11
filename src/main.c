// Common includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Platform-specific includes and definitions
#include "shared/platform/platform.h"

// Imports
#include "server/server.h"
#include "routes/routes.h"
#include "handlers/handlers.h"
#include "shared/http/response.h"
#include "shared/http/network.h"

// Constants
#define BUFFER_SIZE 4096

void handleClient(Server* server, PLATFORM_SOCKET clientConnection) {
    char buffer[BUFFER_SIZE] = {0};
    
    int bytesRead = recv(clientConnection, buffer, BUFFER_SIZE - 1, 0);
    
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        handleRequest(server, clientConnection, buffer);
    }
    
    closeConnection(clientConnection, "");
}

int handleRequest(Server* server, PLATFORM_SOCKET clientConnection, char* request) {
    if (strncmp(request, "GET ", 4) != 0) {
        sendErrorResponse(clientConnection, 405, "Method Not Allowed", "Only GET method is allowed");
        return 0;
    }

    char* path = request + 4;
    char* pathEnd = strchr(path, ' ');
    if (!pathEnd) {
        sendErrorResponse(clientConnection, 400, "Bad Request", "Invalid request format");
        return 0;
    }
    *pathEnd = '\0';

    for (int i = 0; i < server->routeCount; i++) {
        if (strcmp(server->routes[i].path, path) == 0) {
            server->routes[i].handler(server, clientConnection);
            return 1;
        }
    }

    char notFoundMessage[1024];
    snprintf(notFoundMessage, sizeof(notFoundMessage), 
        "{\"error\": \"Route not found\", \"path\": \"%s\"}", path);
    sendErrorResponse(clientConnection, 404, "Not Found", notFoundMessage);
    return 0;
}

int main()
{
    if (platformNetworkingInit() != 0) {
        fprintf(stderr, "Failed to initialize platform networking\n");
        return 1;
    }
    
    Server server;
    startServer(&server, 8080);
    
    if (server.socket < 0) {
        fprintf(stderr, "Failed to initialize server\n");
        platformNetworkingCleanup();
        return 1;
    }
    
    addRoute(&server, "/", rootPathHandler);
    addRoute(&server, "/api/api-test", apiHandler);
    addRoute(&server, "/api/machine", machinesHandler);
    addRoute(&server, "/api/os", osHandler);
    addRoute(&server, "/api/sys", systemInfoHandler);
    
    serverListening(&server, handleClient);
    
    cleanupServer(&server);
    platformNetworkingCleanup();
    
    return 0;
}
