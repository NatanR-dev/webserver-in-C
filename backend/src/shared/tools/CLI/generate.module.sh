#!/bin/bash

# Check if a module name was provided
if [ -z "$1" ]; then
    echo "[ERROR] Usage: $0 <module_name>"
    echo "[ERROR] Example: $0 users"
    exit 1
fi

MODULE_NAME="$1"

# Validate module name
if ! echo "$MODULE_NAME" | grep -qE '^[a-z][a-z0-9_]*$'; then
    echo "[ERROR] Invalid module name. Use only lowercase letters, numbers, and underscores."
    exit 1
fi

# Paths
SCRIPT_DIR="$(dirname "$0")"
TARGET_DIR="$SCRIPT_DIR/../../../../src/$MODULE_NAME"

# Check if the directory already exists
if [ -d "$TARGET_DIR" ]; then
    echo "[ERROR] Module '$MODULE_NAME' already exists at $TARGET_DIR"
    exit 1
fi

# Create the new module directory
echo "[INFO] Creating directory: $TARGET_DIR"
mkdir -p "$TARGET_DIR"
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to create directory $TARGET_DIR"
    exit 1
fi

echo "[INFO] Creating new module: $MODULE_NAME"

# Create boilerplate files
echo "[INFO] Creating $MODULE_NAME.controller.h"
cat > "$TARGET_DIR/$MODULE_NAME.controller.h" << EOF
#ifndef ${MODULE_NAME}_CONTROLLER_H
#define ${MODULE_NAME}_CONTROLLER_H

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"

typedef struct ${MODULE_NAME}Service ${MODULE_NAME}Service;

/// @brief Retrieves the controller routes
/// @param count Pointer to store the number of routes
/// @return Array of route configurations
const RouteConfig* get${MODULE_NAME}Routes(int* count);

/// @brief Retrieves the service instance
/// @return Pointer to the service
${MODULE_NAME}Service* get${MODULE_NAME}Service(void);

/// @brief Handles GET request for /${MODULE_NAME}
/// @param server Pointer to the server
/// @param clientConnection Client connection
void handleGet${MODULE_NAME}(void* server, void* clientConnection);

#endif // ${MODULE_NAME}_CONTROLLER_H
EOF
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to create $MODULE_NAME.controller.h"
    exit 1
fi

echo "[INFO] Creating $MODULE_NAME.controller.c"
cat > "$TARGET_DIR/$MODULE_NAME.controller.c" << EOF
#include "$MODULE_NAME.controller.h"
#include "$MODULE_NAME.service.h"

#include "../../shared/http/response/response.h"
#include "../../shared/http/router/route.decorators.h"
#include "../../shared/http/server/server.h"

#include <stdlib.h>
#include <string.h>

static ${MODULE_NAME}Service* service = NULL;

static const RouteConfig ${MODULE_NAME}Routes[] = {
    GET("/$MODULE_NAME", handleGet${MODULE_NAME})
};

const RouteConfig* get${MODULE_NAME}Routes(int* count) {
    if (count) {
        *count = sizeof(${MODULE_NAME}Routes) / sizeof(RouteConfig);
    }
    return ${MODULE_NAME}Routes;
}

${MODULE_NAME}Service* get${MODULE_NAME}Service(void) {
    return service;
}

void handleGet${MODULE_NAME}(void* serverPtr, void* clientConnectionPtr) {
    Server* server = (Server*)serverPtr;
    PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET)(uintptr_t)clientConnectionPtr;

    char* response = get${MODULE_NAME}Info(get${MODULE_NAME}Service(), server);
    if (response) {
        sendJsonResponse(clientConnection, response);
        free(response);
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "An error occurred while processing your request.");
    }
}
EOF
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to create $MODULE_NAME.controller.c"
    exit 1
fi

echo "[INFO] Creating $MODULE_NAME.service.h"
cat > "$TARGET_DIR/$MODULE_NAME.service.h" << EOF
#ifndef ${MODULE_NAME}_SERVICE_H
#define ${MODULE_NAME}_SERVICE_H

#include "../../shared/http/server/server.h"

typedef struct ${MODULE_NAME}Service {
    // Add necessary fields for the service here
} ${MODULE_NAME}Service;

/// @brief Initializes the service
/// @param service Pointer to the service
void ${MODULE_NAME}ServiceInit(${MODULE_NAME}Service* service);

/// @brief Cleans up service resources
/// @param service Pointer to the service
void ${MODULE_NAME}ServiceCleanup(${MODULE_NAME}Service* service);

/// @brief Retrieves module information for JSON response
/// @param service Pointer to the service
/// @param server Pointer to the server
/// @return Dynamically allocated JSON string (must be freed by the caller)
char* get${MODULE_NAME}Info(${MODULE_NAME}Service* service, Server* server);

#endif // ${MODULE_NAME}_SERVICE_H
EOF
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to create $MODULE_NAME.service.h"
    exit 1
fi

echo "[INFO] Creating $MODULE_NAME.service.c"
cat > "$TARGET_DIR/$MODULE_NAME.service.c" << EOF
#include "$MODULE_NAME.service.h"

#include "../../shared/formats/json/json.h"
#include <stdlib.h>
#include <string.h>

void ${MODULE_NAME}ServiceInit(${MODULE_NAME}Service* service) {
    (void)service;
}

void ${MODULE_NAME}ServiceCleanup(${MODULE_NAME}Service* service) {
    (void)service;
}

char* get${MODULE_NAME}Info(${MODULE_NAME}Service* service, Server* server) {
    (void)service;
    (void)server;

    char* result = malloc(256);
    if (!result) return NULL;

    createJsonObject(result, 256, 2,
        "message", "Hello from ${MODULE_NAME} module!",
        "port", "8080");

    return result;
}
EOF
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to create $MODULE_NAME.service.c"
    exit 1
fi

echo "[INFO] Creating $MODULE_NAME.module.h"
cat > "$TARGET_DIR/$MODULE_NAME.module.h" << EOF
#ifndef ${MODULE_NAME}_MODULE_H
#define ${MODULE_NAME}_MODULE_H

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"

/// @brief Initializes the module
/// @param server Pointer to the server
void ${MODULE_NAME}ModuleInit(Server* server);

/// @brief Cleans up module resources
/// @param server Pointer to the server
void ${MODULE_NAME}ModuleCleanup(Server* server);

#endif // ${MODULE_NAME}_MODULE_H
EOF
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to create $MODULE_NAME.module.h"
    exit 1
fi

echo "[INFO] Creating $MODULE_NAME.module.c"
cat > "$TARGET_DIR/$MODULE_NAME.module.c" << EOF
#include "$MODULE_NAME.module.h"
#include "$MODULE_NAME.controller.h"
#include "$MODULE_NAME.service.h"

#include "../../shared/http/server/server.h"
#include "../../shared/http/router/routes.h"
#include <stdlib.h>

static ${MODULE_NAME}Service* service = NULL;

void ${MODULE_NAME}ModuleInit(Server* server) {
    if (!server) return;

    service = (${MODULE_NAME}Service*)malloc(sizeof(${MODULE_NAME}Service));
    if (!service) return;

    ${MODULE_NAME}ServiceInit(service);

    int count = 0;
    const RouteConfig* routes = get${MODULE_NAME}Routes(&count);
    if (routes && count > 0) {
        for (int i = 0; i < count; i++) {
            addRouteMethod(server, routes[i].path, routes[i].handler, routes[i].method);
        }
    }

    printf("[\\x1b[32mINFO\\x1b[0m] ${MODULE_NAME} module initialized\\n");
}

void ${MODULE_NAME}ModuleCleanup(Server* server) {
    (void)server;

    if (service) {
        ${MODULE_NAME}ServiceCleanup(service);
        free(service);
        service = NULL;
    }
}
EOF
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to create $MODULE_NAME.module.c"
    exit 1
fi

echo
echo "[INFO] Module '$MODULE_NAME' created successfully!"
echo
echo "[INFO] Next steps:"
echo "[INFO] 1. Initialize the ${MODULE_NAME}"
echo "--> Add ModuleInit(&server); function in main.c"
echo "[INFO] 2. Cleanup the module"
echo "--> Add ModuleCleanup(&server); function in main.c"
echo "[INFO] 3. Access the Request.http on root directory"
echo "--> Use GET http://localhost:8080/${MODULE_NAME}"
echo "--> Expected output: {\"message\": \"Hello from ${MODULE_NAME} module!\", \"port\": \"8080\"}"

exit 0