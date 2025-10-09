#include "users.controller.h"
#include "users.service.h"

#include "../../shared/http/response/response.h"
#include "../../shared/http/router/route.decorators.h"
#include "../../shared/http/server/server.h"

#include <stdlib.h>
#include <string.h>

static usersService* service = NULL;

static const RouteConfig usersRoutes[] = {
    GET("/users", handleGetusers)
};

const RouteConfig* getusersRoutes(int* count) {
    if (count) {
        *count = sizeof(usersRoutes) / sizeof(RouteConfig);
    }
    return usersRoutes;
}

usersService* getusersService(void) {
    return service;
}

void handleGetusers(void* serverPtr, void* clientConnectionPtr) {
    Server* server = (Server*)serverPtr;
    PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET)(uintptr_t)clientConnectionPtr;

    char* response = getusersInfo(getusersService(), server);
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "An error occurred while processing your request.");
    }
}
