#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#endif

void jsonEscapeString(const char* input, char* output, size_t outputSize) {
    size_t i = 0;
    size_t j = 0;
    while (input[i] != '\0' && j < outputSize - 1) {
        switch (input[i]) {
            case '\\':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = '\\';
                break;
            case '"':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = '"';
                break;
            case '\b':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = 'b';
                break;
            case '\f':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = 'f';
                break;
            case '\n':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = 'n';
                break;
            case '\r':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = 'r';
                break;
            case '\t':
                output[j++] = '\\';
                if (j < outputSize - 1) output[j++] = 't';
                break;
            default:
                if (input[i] >= ' ' && input[i] <= '~') {
                    output[j++] = input[i];
                } else {
                    snprintf(output + j, outputSize - j, "\\u%04x", input[i]);
                    j += 6;
                }
                break;
        }
        i++;
    }
    output[j] = '\0';
}

void sendHttpResponse(int clientConnection, int statusCode, const char* statusMessage, 
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
        
    #ifdef _WIN32
    send(clientConnection, response, (int)strlen(response), 0);
    #else
    write(clientConnection, response, strlen(response));
    #endif
    
    free(response);
}

void sendJsonResponse(int clientConnection, const char* json) {
    sendHttpResponse(clientConnection, 200, "OK", "application/json", json, "keep-alive");
}

void sendErrorResponse(int clientConnection, int statusCode, 
                      const char* statusMessage, const char* body) {
    sendHttpResponse(clientConnection, statusCode, statusMessage, "text/plain", body, "close");
}
