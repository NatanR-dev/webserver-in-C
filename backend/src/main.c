// Common 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Platform 
#include "shared/platform/platform.h"

// Server
#include "shared/http/server/server.h"

// Modules
#include "root/root.module.h"
#include "system/system.module.h"

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
