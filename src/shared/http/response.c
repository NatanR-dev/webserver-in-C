#include "response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Platform 
#include "../platform/platform.h"

#define SEND_FUNC send

void sendHttpResponse(PLATFORM_SOCKET clientConnection, int statusCode, const char* statusMessage, 
        const char* contentType, const char* body, const char* connection) {
    int requiredSize = snprintf(NULL, 0, 
        "HTTP/1.1 %d %s\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: %s\r\n"
        "\r\n"
        "%s", 
        statusCode, statusMessage, contentType, (int)strlen(body), connection, body) + 1;
        
    char* response = (char*)malloc(requiredSize);
    if (!response) return;
    
    snprintf(response, requiredSize,
        "HTTP/1.1 %d %s\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: %s\r\n"
        "\r\n"
        "%s", 
        statusCode, statusMessage, contentType, (int)strlen(body), connection, body);
    
    send(clientConnection, response, (int)strlen(response), 0);
    free(response);
}

void sendErrorResponse(PLATFORM_SOCKET clientConnection, int statusCode, 
        const char* statusMessage, const char* body) {
    sendHttpResponse(clientConnection, statusCode, statusMessage, 
        "text/plain", body, "close");
}

void sendJsonResponse(PLATFORM_SOCKET clientConnection, const char* json) {
    sendHttpResponse(clientConnection, 200, "OK", 
        "application/json; charset=utf-8", json, "keep-alive");
}
