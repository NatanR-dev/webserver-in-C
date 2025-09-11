#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include <stddef.h>

/**
 * @brief Sends an HTTP response to the client
 * @param clientConnection The client connection socket
 * @param statusCode HTTP status code
 * @param statusMessage HTTP status message
 * @param contentType Content-Type header value
 * @param body Response body
 * @param connection Connection header value (e.g., "keep-alive" or "close")
 */
void sendHttpResponse(int clientConnection, int statusCode, const char* statusMessage, 
    const char* contentType, const char* body, const char* connection);

/**
 * @brief Sends an error response to the client
 * @param clientConnection The client connection socket
 * @param statusCode HTTP status code
 * @param statusMessage HTTP status message
 * @param body Error message body
 */
void sendErrorResponse(int clientConnection, int statusCode, 
    const char* statusMessage, const char* body);

/**
 * @brief Gets the local IP address
 * @param ip Buffer to store the IP address
 * @param ipSize Size of the buffer
 * @return 0 on success, non-zero on error
 */
int getLocalIP(char* ip, size_t ipSize);

#endif 
