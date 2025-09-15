#ifndef SERVER_H
#define SERVER_H

// Common
#include <stddef.h>
#include <stdint.h>

// Platform includes
#include "../shared/platform/platform.h"

// Constants
#define MAX_ROUTES 10

typedef struct Server Server;
typedef struct Route Route;
typedef void (*RouteHandler)(Server*, PLATFORM_SOCKET);

struct Route {
    char* path;
    RouteHandler handler;
};

struct Server {
    PLATFORM_SOCKET socket;
    int port;  
    Route routes[MAX_ROUTES];
    int routeCount;
};

int initializeServer(Server* server, int port);
void cleanupServer(Server* server);
PLATFORM_SOCKET acceptConnection(Server* server);
int addRoute(Server* server, const char* path, RouteHandler handler);
void handleRequest(Server* server, PLATFORM_SOCKET clientConnection, const char* request);

void sendResponse(PLATFORM_SOCKET clientConnection, const char* response);
void sendJsonResponse(PLATFORM_SOCKET clientConnection, const char* json);
void sendError(PLATFORM_SOCKET clientConnection, int statusCode, const char* message);
void sendErrorResponse(PLATFORM_SOCKET clientConnection, int statusCode, const char* statusText, const char* message);
void closeConnection(PLATFORM_SOCKET clientConnection, const char* message);

void initSockets(void);
void cleanupSockets(void);

void serverListening(Server* server, void (*clientHandler)(Server*, PLATFORM_SOCKET));

#endif 
