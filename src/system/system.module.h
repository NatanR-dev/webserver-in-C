#ifndef SYSTEM_MODULE_H
#define SYSTEM_MODULE_H

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"

void systemModuleInit(Server* server);

void systemModuleCleanup(Server* server);

const RouteConfig* getSystemRoutes(int* count);

#endif
