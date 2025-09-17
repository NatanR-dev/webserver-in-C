#ifndef ROUTE_DECORATORS_H
#define ROUTE_DECORATORS_H

#include <stddef.h>

// HTTP Methods
typedef enum {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DEL,
    HTTP_PATCH,
    HTTP_OPTIONS,
    HTTP_HEAD
} HttpMethod;

// Route handler function type
typedef void (*RouteHandlerFunc)(void* server, void* clientConnection);

// Route configuration
#define ROUTE(method, path, handler) \
    { method, path, (RouteHandlerFunc)handler }

// Common HTTP methods and macros for route decorators
#define GET(path, handler)     ROUTE(HTTP_GET, path, handler)
#define POST(path, handler)    ROUTE(HTTP_POST, path, handler)
#define PUT(path, handler)     ROUTE(HTTP_PUT, path, handler)
#define DEL(path, handler)     ROUTE(HTTP_DEL, path, handler)
#define PATCH(path, handler)   ROUTE(HTTP_PATCH, path, handler)
#define OPTIONS(path, handler) ROUTE(HTTP_OPTIONS, path, handler)
#define HEAD(path, handler)    ROUTE(HTTP_HEAD, path, handler)

// Alias for DEL to maintain compatibility with HTTP method names
#ifndef DELETE
#define DELETE DEL
#endif

// Controller decorator (for future use)
#define CONTROLLER(prefix) static const char* _controller_prefix = prefix;

// Route configuration structure
typedef struct {
    HttpMethod method;
    char* path;  // Changed from const char* to char* to allow freeing
    RouteHandlerFunc handler;
} RouteConfig;

#endif 
