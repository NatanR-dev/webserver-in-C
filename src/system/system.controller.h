#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include "../server/server.h"
#include "../shared/router/routes.h"

typedef struct SystemService SystemService;

// Route handlers
extern void handleGetMachineInfo(void* server, void* clientConnection);
extern void handleGetOsInfo(void* server, void* clientConnection);
extern void handleGetSystemInfo(void* server, void* clientConnection);

// Route getter
const RouteConfig* getSystemRoutes(int* count);

// Service getter
SystemService* getSystemService(void);

#endif
