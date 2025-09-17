// Common includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Platform 
#include "../../platform/platform.h"

// Imports
#include "router.h"
#include "route.decorators.h"


int addRouteMethod(Server* server, const char* path, RouteHandlerFunc handler, int method) {
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
    server->routes[server->routeCount].method = method;
    server->routeCount++;
    return 0;
}

