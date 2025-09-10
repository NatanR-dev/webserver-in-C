#ifndef SERVER_H
#define SERVER_H

// Common
#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
// WINDOWS
    #include <winsock2.h>
#else
// UNIX-LIKE
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

// Constants
#define MAX_ROUTES 10

typedef struct Route Route;
typedef struct Server Server;
typedef void (*RouteHandler)(Server*, int);

struct Route {
    char* path;
    RouteHandler handler;
};

struct Server {
    int socket;
    Route routes[MAX_ROUTES];
    int routeCount;
};

void initSockets();
void cleanupSockets();
void closeConnection(int clientConnection, char* response);
void startServer(Server* server);
void cleanupServer(Server* server);
int handleRequest(Server* server, int clientConnection, char* request);

#endif 
