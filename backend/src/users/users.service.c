#include "users.service.h"

#include "../../shared/formats/json/json.h"
#include <stdlib.h>
#include <string.h>

void usersServiceInit(usersService* service) {
    (void)service;
}

void usersServiceCleanup(usersService* service) {
    (void)service;
}

char* getusersInfo(usersService* service, Server* server) {
    (void)service;
    (void)server;

    char* result = malloc(256);
    if (!result) return NULL;

    createJsonObject(result, 256, 2,
        "message", "Hello from users module!",
        "port", "8080");

    return result;
}
