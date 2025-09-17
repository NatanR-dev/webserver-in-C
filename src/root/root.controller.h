#ifndef ROOT_CONTROLLER_H
#define ROOT_CONTROLLER_H

#include "../server/server.h"
#include "../shared/router/routes.h"

typedef struct RootService RootService;

// Route handlers
extern void handleGetRoot(void* server, void* clientConnection);
extern void handleGetApi(void* server, void* clientConnection);

// Route getter
const RouteConfig* getRootRoutes(int* count);

// Service getter
RootService* getRootService(void);

#endif
