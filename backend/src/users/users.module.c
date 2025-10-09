#include "users.module.h"
#include "users.controller.h"
#include "users.service.h"

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"
#include <stdlib.h>

static usersService* service = NULL;

void usersModuleInit(Server* server) {
    if (!server) return;

    service = (usersService*)malloc(sizeof(usersService));
    if (!service) return;

    usersServiceInit(service);

    int count = 0;
    const RouteConfig* routes = getusersRoutes(&count);
    if (routes && count > 0) {
        for (int i = 0; i < count; i++) {
            addRouteMethod(server, routes[i].path, routes[i].handler, routes[i].method);
        }
    }

    printf("[\x1b[32mINFO\x1b[0m] users module initialized\n");
}

void usersModuleCleanup(Server* server) {
    (void)server;

    if (service) {
        usersServiceCleanup(service);
        free(service);
        service = NULL;
    }
}
