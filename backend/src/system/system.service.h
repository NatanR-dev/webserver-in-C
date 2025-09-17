#ifndef SYSTEM_SERVICE_H
#define SYSTEM_SERVICE_H

#include <stddef.h>
#include "../../shared/platform/platform.h"

typedef struct SystemService {
} SystemService;

void systemServiceInit(SystemService* service);
void systemServiceCleanup(SystemService* service);

char* getMachineInfo(SystemService* service);

char* getOsInfo(SystemService* service);

char* getSystemInfo(SystemService* service);

#endif
