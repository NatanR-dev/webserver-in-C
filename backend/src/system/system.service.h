#ifndef SYSTEM_SERVICE_H
#define SYSTEM_SERVICE_H

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../shared/platform/platform.h"

#define MAX_MACHINE_NAMES 100 

typedef struct MachineNames {
    char* names[MAX_MACHINE_NAMES];
    size_t count;
} MachineNames;

typedef struct SystemService {
    char* machineName;
    MachineNames usedNames; 
} SystemService;

void systemServiceInit(SystemService* service);
void systemServiceCleanup(SystemService* service);

/**
 * Validates if a machine name contains only allowed characters (alphanumeric, underscore, hyphen)
 * @param name The name to validate
 * @return 1 if valid, 0 if invalid
 */
int isValidMachineName(const char* name);

/**
 * @brief Sets the machine name if it's valid
 * @param service The system service instance
 * @param name The new machine name to set
 * @return 1 if successful, 0 if the name is invalid
 */
/**
 * @brief Sets the machine name if it's valid and not a duplicate
 * @param service The system service instance
 * @param name The new machine name to set
 * @param error Buffer to store error message if any
 * @param error_size Size of the error buffer
 * @return true if successful, false if the name is invalid or duplicate
 */
bool setMachineName(SystemService* service, const char* name, char* error, size_t error_size);

/**
 * @brief Checks if a machine name already exists
 * @param service The system service instance
 * @param name The name to check
 * @return true if the name exists, false otherwise
 */
bool isMachineNameUsed(const SystemService* service, const char* name);

/**
 * @brief Adds a machine name to the used names list if not already present
 * @param service The system service instance
 * @param name The name to add
 * @return true if added successfully, false if duplicate or error
 */
bool addMachineName(SystemService* service, const char* name);

// Function to get the system service instance
SystemService* getSystemService(void);

char* getMachineInfo(SystemService* service);
char* getOsInfo(SystemService* service);
char* getSystemInfo(SystemService* service);

#endif
