#ifndef VALIDATION_H
#define VALIDATION_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Validates a machine name with detailed error reporting
 * 
 * @param name The name to validate
 * @param error Buffer to store error message (JSON format)
 * @param error_size Size of the error buffer
 * @return int 1 if valid, 0 if invalid
 */
int validateMachineName(const char* name, char* error, size_t error_size);

/**
 * @brief Validates a machine name according to the following rules:
 * - Contain only alphanumeric characters, underscores, or hyphens
 * - Start with an alphanumeric character
 * - End with an alphanumeric character
 * - Be between 1 and 63 characters long
 * 
 * @param name The name to validate
 * @return int 1 if valid, 0 if invalid
 */
int isValidMachineName(const char* name);

#ifdef __cplusplus
}
#endif

#endif // VALIDATION_H
