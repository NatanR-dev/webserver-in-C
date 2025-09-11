// Common includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Platform includes
#include "../shared/platform/platform.h"

// Imports
#include "routes.h"

void addRoute(Server* server, char* path, RouteHandler handler) {
    if (server->routeCount >= MAX_ROUTES) {
        fprintf(stderr, "Error: Maximum number of routes (%d) exceeded\n", MAX_ROUTES);
        return;
    }
    server->routes[server->routeCount].path = strdup(path);
    server->routes[server->routeCount].handler = handler;
    server->routeCount++;
}
