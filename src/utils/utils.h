#ifndef UTILS_H
#define UTILS_H

// Common
#include <stddef.h>

void jsonEscapeString(const char* input, char* output, size_t outputSize);

void sendJsonResponse(int clientConnection, const char* json);
void sendHttpResponse(int clientConnection, int statusCode, const char* statusMessage, 
                     const char* contentType, const char* body, const char* connection);
void sendErrorResponse(int clientConnection, int statusCode, 
                      const char* statusMessage, const char* body);

int getLocalIP(char* ip, size_t ipSize);
void generateMachineId(char* id, size_t idSize);

#endif 
