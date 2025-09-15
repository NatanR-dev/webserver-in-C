#ifndef ROOT_CONTROLLER_H
#define ROOT_CONTROLLER_H

#include "../server/server.h"

typedef struct RootService RootService;

void registerRootRoutes(Server* server, RootService* service);

void rootPathHandler(Server* server, PLATFORM_SOCKET clientConnection);
void apiHandler(Server* server, PLATFORM_SOCKET clientConnection);

#endif 
