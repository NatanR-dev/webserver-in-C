#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include "../server/server.h"

typedef struct SystemService SystemService;

void registerSystemRoutes(Server* server, SystemService* service);

void machinesHandler(Server* server, PLATFORM_SOCKET clientConnection);
void osHandler(Server* server, PLATFORM_SOCKET clientConnection);
void systemInfoHandler(Server* server, PLATFORM_SOCKET clientConnection);

#endif 
