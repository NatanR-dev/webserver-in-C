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
 * @param buffer Buffer to store the JSON string
 * @param bufferSize Size of the buffer
 * @param count Number of key-value pairs (must be even)
 * @param ... Key-value pairs (const char* key, const char* value, ...)
 * @return Pointer to the buffer, or NULL on error
 */
char* createJsonObject(char* buffer, size_t bufferSize, int count, ...);

/**
 * @brief Extracts a string value from a JSON object
 * @param json The JSON string to parse
 * @param key The key to extract
 * @param value Buffer to store the extracted value
 * @param value_size Size of the value buffer
 * @return 1 if successful, 0 on error
 */
int jsonExtractString(const char* json, const char* key, char* value, size_t value_size);

#endif 
