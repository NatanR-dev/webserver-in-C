#ifndef SERVER_H
#define SERVER_H

// Common
#include <stddef.h>
#include <stdint.h>

// Platform 
#include "../shared/platform/platform.h"

// Include route configuration before Server struct
#include "../shared/router/route.decorators.h"

// Constants
#define MAX_ROUTES 50

// Forward declarations
typedef struct Server Server;

typedef struct Server {
    PLATFORM_SOCKET socket;
    int port;  
    RouteConfig routes[MAX_ROUTES];
    int routeCount;
} Server;

// Server lifecycle
int initializeServer(Server* server, int port);
void cleanupServer(Server* server);
void serverListening(Server* server, void (*clientHandler)(Server*, PLATFORM_SOCKET));
PLATFORM_SOCKET acceptConnection(Server* server);

// Route management
int addRoute(Server* server, const char* path, RouteHandlerFunc handler);
int addRouteMethod(Server* server, const char* path, RouteHandlerFunc handler, int method);

// Request handling
void handleRequest(Server* server, PLATFORM_SOCKET clientConnection, const char* request);

// Response helpers
void sendResponse(PLATFORM_SOCKET clientConnection, const char* response);
void sendJsonResponse(PLATFORM_SOCKET clientConnection, const char* json);
void sendError(PLATFORM_SOCKET clientConnection, int statusCode, const char* message);
void sendErrorResponse(PLATFORM_SOCKET clientConnection, int statusCode, const char* statusText, const char* message);
void closeConnection(PLATFORM_SOCKET clientConnection, const char* message);

// Network initialization
void initSockets(void);
void cleanupSockets(void);

#endif 
