#ifndef SERVER_H
#define SERVER_H

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

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
