#ifndef ROUTES_H
#define ROUTES_H

// Platform includes
#include "../shared/platform/platform.h"

// Imports
#include "../server/server.h"

void addRoute(Server* server, char* path, RouteHandler handler);

#endif 
