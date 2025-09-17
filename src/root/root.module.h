#ifndef ROOT_MODULE_H
#define ROOT_MODULE_H

#include "../server/server.h"
#include "../shared/router/routes.h"

// Initialize the root module
void rootModuleInit(Server* server);

// Cleanup the root module
void rootModuleCleanup(Server* server);

// Get the root routes
const RouteConfig* getRootRoutes(int* count);

#endif
