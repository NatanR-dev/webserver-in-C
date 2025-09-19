#include "response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// Platform 
#include "../../platform/platform.h"
#include "../../formats/json/json.h"

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

char* createErrorJson(char* buffer, size_t bufferSize, const char* error) {
    return createJsonObject(buffer, bufferSize, 2,
        "status", "error",
        "message", error);
}

char* createSuccessJson(char* buffer, size_t bufferSize, const char* message) {
    return createJsonObject(buffer, bufferSize, 2,
        "status", "success",
        "message", message);
}

void sendJsonErrorResponse(PLATFORM_SOCKET clientConnection, int statusCode, 
                         const char* statusText, const char* error, bool closeConnection) {
    char jsonBuffer[512];
    if (createErrorJson(jsonBuffer, sizeof(jsonBuffer), error) != NULL) {
        sendHttpResponse(clientConnection, statusCode, statusText, 
            "application/json; charset=utf-8", jsonBuffer, 
            closeConnection ? "close" : "keep-alive");
    } else {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", 
            "Failed to generate error response");
    }
}

void sendJsonSuccessResponse(PLATFORM_SOCKET clientConnection, int statusCode,
                           const char* statusText, const char* data, bool closeConnection) {
    if (data) {
        sendHttpResponse(clientConnection, statusCode, statusText,
                        "application/json", data,
                        closeConnection ? "close" : "keep-alive");
    } else {
        char jsonBuffer[512];
        if (createSuccessJson(jsonBuffer, sizeof(jsonBuffer), "Operation completed successfully") != NULL) {
            sendHttpResponse(clientConnection, statusCode, statusText,
                           "application/json; charset=utf-8", jsonBuffer,
                           closeConnection ? "close" : "keep-alive");
        } else {
            sendErrorResponse(clientConnection, 200, "OK", "Success");
        }
    }
}

void httpNotFound(PLATFORM_SOCKET clientConnection, const char* path) {
    char message[256];
    snprintf(message, sizeof(message), "The requested path '%s' was not found", path);
    sendJsonErrorResponse(clientConnection, 404, "Not Found", message, true);
}
