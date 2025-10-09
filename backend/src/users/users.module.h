#ifndef users_MODULE_H
#define users_MODULE_H

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"

/// @brief Initializes the module
/// @param server Pointer to the server
void usersModuleInit(Server* server);

/// @brief Cleans up module resources
/// @param server Pointer to the server
void usersModuleCleanup(Server* server);

#endif // users_MODULE_H
