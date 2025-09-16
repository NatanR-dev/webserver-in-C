#include "root.module.h"
#include "root.controller.h"
#include "root.service.h"

#include "../server/server.h"

#include <stdlib.h>

static RootService* rootService = NULL;

void rootModuleInit(Server* server) {
    rootService = (RootService*)malloc(sizeof(RootService));
    if (!rootService) return;
    rootServiceInit(rootService);
    
    registerRootRoutes(server, rootService);
}

void rootModuleCleanup(Server* server) {
    (void)server;
    
    if (rootService) {
        rootServiceCleanup(rootService);
        free(rootService);
        rootService = NULL;
    }
}
