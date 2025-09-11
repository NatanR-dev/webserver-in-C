#ifndef SERVER_H
#define SERVER_H

// Common
#include <stddef.h>
#include <stdint.h>

// Platform includes
#include "../shared/platform/platform.h"

// Socket types are now defined in platform.h

// Constants
#define MAX_ROUTES 10

typedef struct Route Route;
typedef struct Server Server;
typedef void (*RouteHandler)(Server*, PLATFORM_SOCKET);

struct Route {
    char* path;
    RouteHandler handler;
};

struct Server {
    int socket;
    int port;  
    Route routes[MAX_ROUTES];
    int routeCount;
};

void initSockets();
void cleanupSockets();
void closeConnection(PLATFORM_SOCKET clientConnection, char* response);
void startServer(Server* server, int port);
void serverListening(Server* server, void (*clientHandler)(Server*, PLATFORM_SOCKET));
void cleanupServer(Server* server);
int handleRequest(Server* server, PLATFORM_SOCKET clientConnection, char* request);

#endif 
