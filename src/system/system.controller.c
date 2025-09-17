#include "system.controller.h"
#include "system.service.h"

#include "../shared/http/response.h"
#include "../shared/router/route.decorators.h"
#include "../server/server.h"

#include <stdlib.h>
#include <string.h>

// Service instance
static SystemService* systemService = NULL;

// Route configuration using the new decorators
static const RouteConfig systemRoutes[] = {
    GET("/api/machine", handleGetMachineInfo),
    GET("/api/os", handleGetOsInfo),
    GET("/api/sys", handleGetSystemInfo)
};

// Route getter implementation
const RouteConfig* getSystemRoutes(int* count) {
    if (count) {
        *count = sizeof(systemRoutes) / sizeof(RouteConfig);
    }
    return systemRoutes;
}

// Service getter implementation
SystemService* getSystemService(void) {
    return systemService;
}

// Route handlers implementation
void handleGetMachineInfo(void* serverPtr, void* clientConnectionPtr) {
    PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET)(uintptr_t)clientConnectionPtr;
    (void)serverPtr;
    
    char* response = getMachineInfo(getSystemService());
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "Failed to generate machine info");
    }
}

void handleGetOsInfo(void* serverPtr, void* clientConnectionPtr) {
    PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET)(uintptr_t)clientConnectionPtr;
    (void)serverPtr;
    
    char* response = getOsInfo(getSystemService());
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "Failed to generate OS info");
    }
}

void handleGetSystemInfo(void* serverPtr, void* clientConnectionPtr) {
    PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET)(uintptr_t)clientConnectionPtr;
    (void)serverPtr;
    
    char* response = getSystemInfo(getSystemService());
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "Failed to generate system info");
    }
}
