#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void jsonEscapeString(const char* input, char* output, size_t outputSize);
void sendJsonResponse(int clientConnection, const char* json);
void sendHttpResponse(int clientConnection, int statusCode, const char* statusMessage, 
                     const char* contentType, const char* body, const char* connection);
void sendErrorResponse(int clientConnection, int statusCode, 
                      const char* statusMessage, const char* body);

#endif 
