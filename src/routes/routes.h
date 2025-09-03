#ifndef ROUTES_H
#define ROUTES_H

#include "../server/server.h"

void addRoute(Server* server, char* path, RouteHandler handler);

#endif 
