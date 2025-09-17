#ifndef ROOT_MODULE_H
#define ROOT_MODULE_H

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"

void rootModuleInit(Server* server);
void rootModuleCleanup(Server* server);

const RouteConfig* getRootRoutes(int* count);

#endif
