#ifndef ROOT_CONTROLLER_H
#define ROOT_CONTROLLER_H

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"

typedef struct RootService RootService;

extern void handleGetRoot(void* server, void* clientConnection);
extern void handleGetApi(void* server, void* clientConnection);

const RouteConfig* getRootRoutes(int* count);
RootService* getRootService(void);

#endif
