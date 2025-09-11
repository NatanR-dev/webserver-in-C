#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <stddef.h>

// Platform includes
#include "../../platform/platform.h"

// Socket types are now defined in platform.h

/**
 * @brief Escapes a string for JSON output
 * @param input The input string to escape
 * @param output Buffer to store the escaped string
 * @param outputSize Size of the output buffer
 */
void jsonEscapeString(const char* input, char* output, size_t outputSize);

/**
 * @brief Creates a JSON object string from key-value pairs
 * @param buffer The buffer to store the JSON string
 * @param bufferSize Size of the buffer
 * @param count Number of key-value pairs
 * @param ... Key-value pairs as const char* (key1, value1, key2, value2, ...)
 * @return int Number of characters written (excluding null terminator)
 */
int createJsonObject(char* buffer, size_t bufferSize, int count, ...);

#endif // JSON_UTILS_H
