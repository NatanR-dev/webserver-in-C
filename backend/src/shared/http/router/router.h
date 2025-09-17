#ifndef ROUTER_H
#define ROUTER_H

// Platform 
#include "../../platform/platform.h"

// Imports
#include "../server/server.h"
#include "route.decorators.h"

// RouteHandlerFunc is defined in route.decorators.h

/**
 * @brief Adds a new route to the server with explicit HTTP method
 * 
 * @param server The server instance
 * @param path The URL path to handle
 * @param handler The handler function for the route
 * @param method HTTP method for the route (from HttpMethod enum)
 * @return int 0 on success, -1 on failure
 */
int addRouteMethod(Server* server, const char* path, RouteHandlerFunc handler, int method);

/**
 * @brief Adds a new GET route to the server
 */
#define addRoute(server, path, handler) addRouteMethod(server, path, (RouteHandlerFunc)handler, HTTP_GET)

/**
 * @brief Registers multiple routes at once
 * 
 * @param server The server instance
 * @param routes Array of RouteConfig structures
 * @param count Number of routes in the array
 */
void registerRoutes(Server* server, const RouteConfig* routes, int count);

#endif
