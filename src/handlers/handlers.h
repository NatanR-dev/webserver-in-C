#ifndef HANDLERS_H
#define HANDLERS_H

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

#include "../server/server.h"

void rootPathHandler(Server* server, int clientConnection);
void apiHandler(Server* server, int clientConnection);
void osHandler(Server* server, int clientConnection);
void systemInfoHandler(Server* server, int clientConnection);

#endif 
