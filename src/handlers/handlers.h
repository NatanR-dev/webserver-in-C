#ifndef HANDLERS_H
#define HANDLERS_H

// Constants
#ifndef BUFFER_SIZE
    #define BUFFER_SIZE 4096
#endif

// Imports
#include "../server/server.h"

void rootPathHandler(Server* server, int clientConnection);
void apiHandler(Server* server, int clientConnection);
void machinesHandler(Server* server, int clientConnection);
void osHandler(Server* server, int clientConnection);
void systemInfoHandler(Server* server, int clientConnection);

#endif 
