// Common includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Platform includes
#include "../shared/platform/platform.h"

// Imports
#include "routes.h"

int addRoute(Server* server, const char* path, RouteHandler handler) {
    if (!server || !path || !handler) {
        return -1;
    }
    
    if (server->routeCount >= MAX_ROUTES) {
        fprintf(stderr, "Error: Maximum number of routes (%d) exceeded\n", MAX_ROUTES);
        return -1;
    }
    
    server->routes[server->routeCount].path = strdup(path);
    if (!server->routes[server->routeCount].path) {
        return -1;
    }
    
    server->routes[server->routeCount].handler = handler;
    server->routeCount++;
    return 0;
}
