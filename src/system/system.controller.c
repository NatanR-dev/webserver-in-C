#include "system.controller.h"
#include "system.service.h"
#include "../shared/http/response.h"
#include "../routes/routes.h"
#include <stdlib.h>

static SystemService* systemService = NULL;

void registerSystemRoutes(Server* server, SystemService* service) {
    systemService = service;
    
    addRoute(server, "/api/machine", machinesHandler);
    addRoute(server, "/api/os", osHandler);
    addRoute(server, "/api/sys", systemInfoHandler);
}

void machinesHandler(Server* server, PLATFORM_SOCKET clientConnection) {
    (void)server; 
    
    char* response = getMachineInfo(systemService);
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "Failed to generate response");
    }
}

void osHandler(Server* server, PLATFORM_SOCKET clientConnection) {
    (void)server; 
    
    char* response = getOsInfo(systemService);
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "Failed to generate response");
    }
}

void systemInfoHandler(Server* server, PLATFORM_SOCKET clientConnection) {
    (void)server; 
    
    char* response = getSystemInfo(systemService);
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "Failed to generate response");
    }
}
