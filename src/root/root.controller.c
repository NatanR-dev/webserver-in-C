#include "root.controller.h"
#include "root.service.h"

#include "../shared/http/response.h"
#include "../shared/router/router.h"

#include <stdlib.h>

static RootService* rootService = NULL;

void registerRootRoutes(Server* server, RootService* service) {
    rootService = service;
    
    addRoute(server, "/", rootPathHandler);
    addRoute(server, "/api", apiHandler);
}

void rootPathHandler(Server* server, PLATFORM_SOCKET clientConnection) {
    char* response = getRootInfo(rootService, server);
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "An error occurred while processing your request.");
    }
}

void apiHandler(Server* server, PLATFORM_SOCKET clientConnection) {
    (void)server; 
    char* response = getApiInfo(rootService);
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "An error occurred while processing your request.");
    }
}

