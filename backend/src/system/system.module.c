#include "system.module.h"
#include "system.controller.h"
#include "system.service.h"

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"
#include "../../shared/http/response/response.h"

#include <stdlib.h>

extern SystemService* systemService; 

void systemModuleInit(Server* server) {
    if (!server) return;
    
    if (!systemService) {
        systemService = (SystemService*)malloc(sizeof(SystemService));
        if (!systemService) return;
        systemServiceInit(systemService);
    }
    
    int count = 0;
    const RouteConfig* routes = getSystemRoutes(&count);
    if (routes && count > 0) {
        for (int i = 0; i < count; i++) {
            addRouteMethod(server, routes[i].path, routes[i].handler, routes[i].method);
        }
    }
    
    printf("LOG: System module initialized\n");
}

void systemModuleCleanup(Server* server) {
    (void)server; 
    
    if (systemService) {
        systemServiceCleanup(systemService);
        free(systemService);
        systemService = NULL;
        printf("LOG: System module cleaned up\n");
    }
}
