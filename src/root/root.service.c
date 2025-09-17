#include "root.service.h"

#include "../../shared/formats/json/json.h"

#include <string.h>
#include <stdlib.h>

void rootServiceInit(RootService* service) {
    (void)service;
}

void rootServiceCleanup(RootService* service) {
    (void)service;
}

char* getRootInfo(RootService* service, Server* server) {
    (void)service;
    
    char* result = malloc(8192);
    if (!result) return NULL;
    
    char routes[8192] = "[";
    size_t offset = 1; 
    
    for (int i = 0; i < server->routeCount; i++) {
        char fullUrl[2048];
        snprintf(fullUrl, sizeof(fullUrl), "http://localhost:8080%s", 
                server->routes[i].path);
        
        char routeObj[4096];
        createJsonObject(routeObj, sizeof(routeObj), 2,
            "path", server->routes[i].path,
            "link", fullUrl);
        
        if (i > 0) {
            offset += snprintf(routes + offset, sizeof(routes) - offset, ",\n        ");
        } else {
            offset += snprintf(routes + offset, sizeof(routes) - offset, "\n        ");
        }
        
        offset += snprintf(routes + offset, sizeof(routes) - offset, "%s", routeObj);
    }
    
    snprintf(routes + offset, sizeof(routes) - offset, "\n    ]");
    
    snprintf(result, 8192,
        "{\n"
        "   \"message\": \"Welcome to low-level C API\",\n"
        "   \"version\": \"1.0\",\n"
        "   \"quick_start\": %s\n"
        "}",
        routes);
        
    return result;
}

char* getApiInfo(RootService* service) {
    (void)service;
    
    char* result = malloc(256);
    if (!result) return NULL;
    
    createJsonObject(result, 256, 2,
        "message", "Hello from my API!",
        "port", "8080");
        
    return result;
}
