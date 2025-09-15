#include "system.module.h"
#include "system.controller.h"
#include "system.service.h"
#include "../server/server.h"
#include <stdlib.h>

static SystemService* systemService = NULL;

void systemModuleInit(Server* server) {
    systemService = (SystemService*)malloc(sizeof(SystemService));
    
    if (!systemService) return;
    systemServiceInit(systemService);
    
    registerSystemRoutes(server, systemService);
}

void systemModuleCleanup(Server* server) {
    (void)server; 
    
    if (systemService) {
        free(systemService);
        systemService = NULL;
    }
}
