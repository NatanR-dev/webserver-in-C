#include <stdio.h>
#include <string.h>
#include "server/server.h"
#include "routes/routes.h"
#include "handlers/handlers.h"
#include "utils/utils.h"

#define BUFFER_SIZE 4096

void handleClient(Server* server, int clientConnection) {
    char buffer[BUFFER_SIZE] = {0};
    
    #ifdef _WIN32
    int bytesRead = recv(clientConnection, buffer, BUFFER_SIZE - 1, 0);
    #else
    int bytesRead = read(clientConnection, buffer, BUFFER_SIZE - 1);
    #endif
    
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        handleRequest(server, clientConnection, buffer);
    }
    
    closeConnection(clientConnection, "");
}

int handleRequest(Server* server, int clientConnection, char* request) {
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

int main() {
    initSockets();
    
    Server server = {0};
    
    addRoute(&server, "/", rootPathHandler);
    addRoute(&server, "/api", apiHandler);
    addRoute(&server, "/os", osHandler);
    addRoute(&server, "/sys", systemInfoHandler);
    
    startServer(&server);
    cleanupServer(&server);
    cleanupSockets();
    
    return 0;
}
