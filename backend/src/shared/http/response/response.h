#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <stddef.h>
#include <stdbool.h>

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

/**
 * @brief Sends a 404 Not Found response with JSON body
 * @param clientConnection The client connection socket
 * @param path The requested path that was not found
 */
void httpNotFound(PLATFORM_SOCKET clientConnection, const char* path);

/**
 * @brief Sends a JSON error response with the given status code and message
 * @param clientConnection The client connection socket
 * @param statusCode HTTP status code
 * @param statusText HTTP status text
 * @param error Error message
 * @param closeConnection Whether to close the connection after sending
 */
void sendJsonErrorResponse(PLATFORM_SOCKET clientConnection, int statusCode, 
    const char* statusText, const char* error, bool closeConnection);

/**
 * @brief Sends a JSON success response with the given status code and data
 * @param clientConnection The client connection socket
 * @param statusCode HTTP status code (e.g., 200, 201)
 * @param statusText HTTP status text (e.g., "OK", "Created")
 * @param data JSON data string (can be NULL)
 * @param closeConnection Whether to close the connection after sending
 */
void sendJsonSuccessResponse(PLATFORM_SOCKET clientConnection, int statusCode,
    const char* statusText, const char* data, bool closeConnection);

/**
 * @brief Creates a JSON error response
 * @param buffer Buffer to store the JSON response
 * @param bufferSize Size of the buffer
 * @param error Error message
 * @return Pointer to the buffer containing the JSON string, or NULL on error
 */
char* createErrorJson(char* buffer, size_t bufferSize, const char* error);

/**
 * @brief Creates a JSON success response
 * @param buffer Buffer to store the JSON response
 * @param bufferSize Size of the buffer
 * @param message Success message
 * @return Pointer to the buffer containing the JSON string, or NULL on error
 */
char* createSuccessJson(char* buffer, size_t bufferSize, const char* message);

#endif 
