#ifndef users_SERVICE_H
#define users_SERVICE_H

#include "../../shared/http/server/server.h"

typedef struct usersService {
    // Add necessary fields for the service here
} usersService;

/// @brief Initializes the service
/// @param service Pointer to the service
void usersServiceInit(usersService* service);

/// @brief Cleans up service resources
/// @param service Pointer to the service
void usersServiceCleanup(usersService* service);

/// @brief Retrieves module information for JSON response
/// @param service Pointer to the service
/// @param server Pointer to the server
/// @return Dynamically allocated JSON string (must be freed by the caller)
char* getusersInfo(usersService* service, Server* server);

#endif // users_SERVICE_H
