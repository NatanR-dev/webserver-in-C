#ifndef SYSTEM_MODULE_H
#define SYSTEM_MODULE_H

#include "../server/server.h"
#include "../shared/router/routes.h"

// Initialize the system module
void systemModuleInit(Server* server);

// Cleanup the system module
void systemModuleCleanup(Server* server);

// Get the system routes
const RouteConfig* getSystemRoutes(int* count);

#endif
