#ifndef users_CONTROLLER_H
#define users_CONTROLLER_H

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"

typedef struct usersService usersService;

/// @brief Retrieves the controller routes
/// @param count Pointer to store the number of routes
/// @return Array of route configurations
const RouteConfig* getusersRoutes(int* count);

/// @brief Retrieves the service instance
/// @return Pointer to the service
usersService* getusersService(void);

/// @brief Handles GET request for /users
/// @param server Pointer to the server
/// @param clientConnection Client connection
void handleGetusers(void* server, void* clientConnection);

#endif // users_CONTROLLER_H
