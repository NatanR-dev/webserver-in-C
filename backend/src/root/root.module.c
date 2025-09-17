#include "root.module.h"
#include "root.controller.h"
#include "root.service.h"

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"
#include "../../shared/http/response/response.h"

#include <stdlib.h>

static RootService* rootService = NULL;

void rootModuleInit(Server* server) {
    if (!server) return;
    
    rootService = (RootService*)malloc(sizeof(RootService));
    if (!rootService) return;
    
    rootServiceInit(rootService);
    
    int count = 0;
    const RouteConfig* routes = getRootRoutes(&count);
    if (routes && count > 0) {
        for (int i = 0; i < count; i++) {
            addRouteMethod(server, routes[i].path, routes[i].handler, routes[i].method);
        }
    }
}

void rootModuleCleanup(Server* server) {
    (void)server;
    
    if (rootService) {
        rootServiceCleanup(rootService);
        free(rootService);
        rootService = NULL;
    }
}
