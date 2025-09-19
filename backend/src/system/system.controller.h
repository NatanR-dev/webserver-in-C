#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"

typedef struct SystemService SystemService;

extern SystemService* systemService;

extern void handleGetMachineInfo(void* server, void* clientConnection);
extern void handlePostMachineName(void* server, void* clientConnection);
extern void handleGetOsInfo(void* server, void* clientConnection);
extern void handleGetSystemInfo(void* server, void* clientConnection);

const RouteConfig* getSystemRoutes(int* count);

SystemService* getSystemService(void);

#endif
