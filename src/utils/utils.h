#ifndef UTILS_H
#define UTILS_H

// Common
#include <stddef.h>

/**
 * @brief Generates a unique machine ID
 * @param id Buffer to store the machine ID
 * @param idSize Size of the buffer
 */
void generateMachineId(char* id, size_t idSize);

#endif 
