#ifndef ROOT_SERVICE_H
#define ROOT_SERVICE_H

#include "../../shared/http/server/server.h"

typedef struct RootService {
} RootService;

void rootServiceInit(RootService* service);
void rootServiceCleanup(RootService* service);

char* getRootInfo(RootService* service, Server* server);
char* getApiInfo(RootService* service);

#endif
