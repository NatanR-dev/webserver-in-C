#include "root.controller.h"
#include "root.service.h"

#include "../../shared/http/response/response.h"
#include "../../shared/http/router/route.decorators.h"
#include "../../shared/http/server/server.h"

#include <stdlib.h>
#include <string.h>

static RootService* rootService = NULL;

static const RouteConfig rootRoutes[] = {
    GET("/", handleGetRoot),
    GET("/api", handleGetApi)
};

const RouteConfig* getRootRoutes(int* count) {
    if (count) {
        *count = sizeof(rootRoutes) / sizeof(RouteConfig);
    }
    return rootRoutes;
}

RootService* getRootService(void) {
    return rootService;
}
void handleGetRoot(void* serverPtr, void* clientConnectionPtr) {
    Server* server = (Server*)serverPtr;
    PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET)(uintptr_t)clientConnectionPtr;
    
    char* response = getRootInfo(getRootService(), server);
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "An error occurred while processing your request.");
    }
}

void handleGetApi(void* serverPtr, void* clientConnectionPtr) {
    PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET)(uintptr_t)clientConnectionPtr;
    (void)serverPtr;
    
    char* response = getApiInfo(getRootService());
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "An error occurred while processing your request.");
    }
}