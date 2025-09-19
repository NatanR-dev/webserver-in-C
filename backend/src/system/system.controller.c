#include "system.controller.h"
#include "system.service.h"

#include "../../shared/http/response/response.h"
#include "../../shared/http/router/route.decorators.h"
#include "../../shared/validation/validation.h"
#include "../../shared/formats/json/json.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

SystemService* systemService = NULL;

void handleGetMachineInfo(void* server, void* clientConnection);
void handlePostMachineName(void* server, void* clientConnection);
void handleGetOsInfo(void* server, void* clientConnection);
void handleGetSystemInfo(void* server, void* clientConnection);

static const RouteConfig systemRoutes[] = {
    GET("/api/machine", handleGetMachineInfo),
    POST("/api/machine", handlePostMachineName),
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

void handlePostMachineName(void* serverPtr, void* clientConnectionPtr) {
    printf("LOG: Entering handlePostMachineName\n");
    PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET)(uintptr_t)clientConnectionPtr;
    Server* server = (Server*)serverPtr;
    
    if (!server) {
        printf("ERROR: Server pointer is NULL\n");
        sendJsonErrorResponse(clientConnection, 500, "Internal Server Error", 
                            "Server context error", true);
        return;
    }
    
    if (!server->requestBody) {
        printf("ERROR: No request body\n");
        sendJsonErrorResponse(clientConnection, 400, "Bad Request", 
                            "No request body provided", true);
        return;
    }
    
    printf("LOG: Request body: %s\n", server->requestBody);
    
    char nameBuf[256] = {0};
    if (!jsonExtractString(server->requestBody, "machineName", nameBuf, sizeof(nameBuf))) {
        printf("ERROR: Failed to extract machineName from request\n");
        sendJsonErrorResponse(clientConnection, 400, "Bad Request", 
                            "Missing or invalid required field: machineName", true);
        return;
    }
    
    printf("LOG: Extracted machine name: '%s'\n", nameBuf);
    
    char errorMsg[512] = {0};
    if (!validateMachineName(nameBuf, errorMsg, sizeof(errorMsg))) {
        printf("ERROR: %s\n", errorMsg);
        sendJsonErrorResponse(clientConnection, 400, "Bad Request", errorMsg + 10, false);
        return;
    }
    
    SystemService* service = getSystemService();
    if (!service) {
        printf("ERROR: System service not initialized\n");
        sendJsonErrorResponse(clientConnection, 500, "Internal Server Error",
                            "System service not initialized", true);
        return;
    }
    
    if (!setMachineName(service, nameBuf, errorMsg, sizeof(errorMsg))) {
        printf("ERROR: %s\n", errorMsg);
        int statusCode = (strstr(errorMsg, "already in use") != NULL) ? 409 : 400;
        bool closeConnection = (statusCode != 409); 
        const char* statusText = (statusCode == 409) ? "Conflict" : "Bad Request";
        
        sendHttpResponse(clientConnection, statusCode, statusText, 
                        "application/json", errorMsg, 
                        closeConnection ? "close" : "keep-alive");
        return;
    }
    
    printf("LOG: Machine name updated to: %s\n", nameBuf);
    
    char response[256];
    if (!createSuccessJson(response, sizeof(response), 
                         "Machine name updated successfully")) {
        printf("ERROR: Failed to create success JSON\n");
        sendJsonErrorResponse(clientConnection, 500, "Internal Server Error",
                            "Failed to generate response", true);
        return;
    }
    
    printf("LOG: Sending success response\n");
    sendJsonSuccessResponse(clientConnection, 201, "Created", response, false);
    printf("LOG: Successfully processed POST request\n");
    
    #ifdef _WIN32
        closesocket(clientConnection);
    #else
        close(clientConnection);
    #endif
}

void handleGetMachineInfo(void* serverPtr, void* clientConnectionPtr) {
    PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET)(uintptr_t)clientConnectionPtr;
    (void)serverPtr; 
    
    SystemService* service = getSystemService();
    if (!service) {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "System service not initialized");
        return;
    }
    
    char* response = getMachineInfo(service);
    if (response) {
        sendHttpResponse(clientConnection, 200, "OK", "application/json", response, "keep-alive");
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "Failed to get machine info");
    }
}

void handleGetOsInfo(void* serverPtr, void* clientConnectionPtr) {
    PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET)(uintptr_t)clientConnectionPtr;
    (void)serverPtr; 
    
    SystemService* service = getSystemService();
    if (!service) {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "System service not initialized");
        return;
    }
    
    char* response = getOsInfo(service);
    if (response) {
        sendHttpResponse(clientConnection, 200, "OK", "application/json", response, "keep-alive");
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "Failed to get OS info");
    }
}

void handleGetSystemInfo(void* serverPtr, void* clientConnectionPtr) {
    PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET)(uintptr_t)clientConnectionPtr;
    (void)serverPtr; 
    
    SystemService* service = getSystemService();
    if (!service) {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "System service not initialized");
        return;
    }
    
    char* response = getSystemInfo(service);
    if (response) {
        sendHttpResponse(clientConnection, 200, "OK", "application/json", response, "keep-alive");
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "Failed to get system info");
    }
}
