#include "system.module.h"
#include "system.controller.h"
#include "system.service.h"

#include "../server/server.h"
#include "../shared/router/routes.h"
#include "../shared/http/response.h"

#include <stdlib.h>

// Service instance
static SystemService* systemService = NULL;

void systemModuleInit(Server* server) {
    if (!server) return;
    
    // Initialize service
    systemService = (SystemService*)malloc(sizeof(SystemService));
    if (!systemService) return;
    
    systemServiceInit(systemService);
    
    // Register all routes from controller
    int count = 0;
    const RouteConfig* routes = getSystemRoutes(&count);
    if (routes && count > 0) {
        for (int i = 0; i < count; i++) {
            addRouteMethod(server, routes[i].path, routes[i].handler, routes[i].method);
        }
    }
}

void systemModuleCleanup(Server* server) {
    (void)server; 
    
    if (systemService) {
        systemServiceCleanup(systemService);
        free(systemService);
        systemService = NULL;
    }
}
