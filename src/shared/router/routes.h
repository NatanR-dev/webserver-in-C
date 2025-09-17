#ifndef ROUTES_H
#define ROUTES_H

#include "../server/server.h"
#include "../platform/platform.h"  // For PLATFORM_SOCKET
#include "route.decorators.h"

// Use the RouteConfig from route.decorators.h
// This file is kept for backward compatibility

/**
 * @brief Registers multiple routes with the server
 * 
 * @param server The server instance
 * @param routes Array of RouteConfig structures
 * @param count Number of routes in the array
 */
void registerRoutes(Server* server, const RouteConfig* routes, int count);

#endif
