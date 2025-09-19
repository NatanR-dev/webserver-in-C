#include "server.h"
#include "../response/response.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void handleClient(Server* server, PLATFORM_SOCKET clientConnection) {
    size_t bufferSize = BUFFER_SIZE;
    char* request = (char*)malloc(bufferSize);
    if (!request) {
        sendErrorResponse(clientConnection, 500, "Internal Server Error", "Out of memory");
        closeConnection(clientConnection, NULL);
        return;
    }

    size_t totalBytesRead = 0;
    ssize_t bytesRead = 0;
    
    while (1) {
        if (totalBytesRead >= bufferSize - 1) {
            size_t newSize = bufferSize * 2;
            char* newBuffer = (char*)realloc(request, newSize);
            if (!newBuffer) {
                sendErrorResponse(clientConnection, 500, "Internal Server Error", "Out of memory");
                free(request);
                closeConnection(clientConnection, NULL);
                return;
            }
            request = newBuffer;
            bufferSize = newSize;
        }

        bytesRead = recv(clientConnection, request + totalBytesRead, bufferSize - totalBytesRead - 1, 0);
        
        if (bytesRead <= 0) {
            break;
        }
        
        totalBytesRead += bytesRead;
        request[totalBytesRead] = '\0';
        
        if (strstr(request, "\r\n\r\n") != NULL) {
            if (strstr(request, "POST") == request || strstr(request, "POST") == request + 1) {
                const char* contentLengthHeader = "Content-Length: ";
                char* contentLengthStart = strstr(request, contentLengthHeader);
                
                if (contentLengthStart) {
                    contentLengthStart += strlen(contentLengthHeader);
                    int contentLength = atoi(contentLengthStart);
                    
                    char* bodyStart = strstr(request, "\r\n\r\n");
                    if (bodyStart) {
                        bodyStart += 4; 
                        int bodyLength = totalBytesRead - (bodyStart - request);
                        
                        if (bodyLength < contentLength) {
                            continue;
                        }
                    }
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }
    
    if (totalBytesRead > 0) {
        request[totalBytesRead] = '\0';
        handleRequest(server, clientConnection, request);
    } else {
        sendErrorResponse(clientConnection, 400, "Bad Request", "Empty request");
    }
    
    free(request);
    closeConnection(clientConnection, NULL);
}
