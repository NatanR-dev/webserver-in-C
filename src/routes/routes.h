#ifndef ROUTES_H
#define ROUTES_H

// Platform includes
#include "../shared/platform/platform.h"

// Imports
#include "../server/server.h"

int addRoute(Server* server, const char* path, RouteHandler handler);

#endif 
