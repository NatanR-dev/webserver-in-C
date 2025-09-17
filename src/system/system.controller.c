#include "system.controller.h"
#include "system.service.h"

#include "../../shared/http/response/response.h"
#include "../../shared/http/router/route.decorators.h"
#include "../../shared/http/server/server.h"

#include <stdlib.h>
#include <string.h>

static SystemService* systemService = NULL;

static const RouteConfig systemRoutes[] = {
    GET("/api/machine", handleGetMachineInfo),
    GET("/api/os", handleGetOsInfo),
    GET("/api/sys", handleGetSystemInfo)
};

const RouteConfig* getSystemRoutes(int* count) {
    if (count) {
        *count = sizeof(systemRoutes) / sizeof(RouteConfig);
    }
    return systemRoutes;
}

SystemService* getSystemService(void) {
    return systemService;
}
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
