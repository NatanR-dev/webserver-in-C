#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <stddef.h>

// Platform 
#include "../../platform/platform.h"

/**
 * @brief Sends an HTTP response to the client
 * @param clientConnection The client connection socket
 * @param statusCode HTTP status code
 * @param statusMessage HTTP status message
 * @param contentType Content-Type header value
 * @param body Response body
 * @param connection Connection header value (e.g., "keep-alive" or "close")
 */
void sendHttpResponse(PLATFORM_SOCKET clientConnection, int statusCode, const char* statusMessage, 
    const char* contentType, const char* body, const char* connection);

/**
 * @brief Sends an error response to the client
 * @param clientConnection The client connection socket
 * @param statusCode HTTP status code
 * @param statusMessage HTTP status message
 * @param body Error message body
 */
void sendErrorResponse(PLATFORM_SOCKET clientConnection, int statusCode, 
    const char* statusMessage, const char* body);

/**
 * @brief Sends a JSON response to the client
 * @param clientConnection The client connection socket
 * @param json JSON string to send
 */
void sendJsonResponse(PLATFORM_SOCKET clientConnection, const char* json);

#endif 
