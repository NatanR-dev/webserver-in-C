// Common includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Platform-specific includes and definitions
#include "shared/platform/platform.h"
#include "shared/http/response.h"

// Imports
#include "server/server.h"
#include "root/root.module.h"
#include "system/system.module.h"

// Constants
#define BUFFER_SIZE 4096

void handleClient(Server* server, PLATFORM_SOCKET clientConnection) {
    char buffer[BUFFER_SIZE] = {0};
    
    int bytesRead = recv(clientConnection, buffer, BUFFER_SIZE - 1, 0);
    
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        handleRequest(server, clientConnection, buffer);
    }
    
    closeConnection(clientConnection, NULL);
}

int main()
{
    initSockets();
    
    Server server;
    if (initializeServer(&server, 8080) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }

    rootModuleInit(&server);
    systemModuleInit(&server);

    printf("Server started on port %d, waiting for connections...\n", server.port);
    
    while (1) {
        PLATFORM_SOCKET clientConnection = acceptConnection(&server);
        if (clientConnection != INVALID_PLATFORM_SOCKET) {
            handleClient(&server, clientConnection);
        } else {
            char errorMsg[256];
            platformGetErrorString(platformGetLastError(), errorMsg, sizeof(errorMsg));
            fprintf(stderr, "accept failed: %s\n", errorMsg);
        }
    }

    systemModuleCleanup(&server);
    rootModuleCleanup(&server);
    cleanupServer(&server);
    cleanupSockets();
    return 0;
}
