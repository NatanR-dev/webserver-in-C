/**
 * @file routes.c
 * @brief Implementation of route registration functions
 */

#include "routes.h"
#include "router.h"

/**
 * @brief Registers multiple routes with the server
 * 
 * @param server The server instance
 * @param routes Array of RouteConfig structures
 * @param count Number of routes in the array
 */
void registerRoutes(Server* server, const RouteConfig* routes, int count) {
    // Validate input parameters
    if (!server || !routes || count <= 0) {
        return;
    }
    
    // Register each route in the array
    for (int i = 0; i < count; i++) {
        if (routes[i].path && routes[i].handler) {
            addRoute(server, routes[i].path, routes[i].handler);
        }
    }
}
