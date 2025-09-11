#ifndef HANDLERS_H
#define HANDLERS_H

// Constants
#ifndef BUFFER_SIZE
    #define BUFFER_SIZE 4096
#endif

// Imports
#include "../server/server.h"

void rootPathHandler(Server* server, PLATFORM_SOCKET clientConnection);
void apiHandler(Server* server, PLATFORM_SOCKET clientConnection);
void machinesHandler(Server* server, PLATFORM_SOCKET clientConnection);
void osHandler(Server* server, PLATFORM_SOCKET clientConnection);
void systemInfoHandler(Server* server, PLATFORM_SOCKET clientConnection);

#endif 
