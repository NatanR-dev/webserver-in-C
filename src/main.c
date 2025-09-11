// Common
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    // WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
    #define close closesocket
    
    // LINKING LIBS
    #ifdef _MSC_VER
        #pragma comment(lib, "ws2_32.lib")
        #pragma comment(lib, "iphlpapi.lib")
    #endif
#else
    // UNIX-LIKE
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif

// Imports
#include "server/server.h"
#include "routes/routes.h"
#include "handlers/handlers.h"
#include "shared/http/response.h"
#include "shared/http/network.h"

// Constants
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
    addRoute(&server, "/api/api-test", apiHandler);
    addRoute(&server, "/api/machine", machinesHandler);
    addRoute(&server, "/api/os", osHandler);
    addRoute(&server, "/api/sys", systemInfoHandler);
    
    startServer(&server);
    cleanupServer(&server);
    cleanupSockets();
    
    return 0;
}
