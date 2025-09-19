#include "validation.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define MAX_ERROR_LENGTH 256

int validateMachineName(const char* name, char* error, size_t error_size) {
    if (!name || *name == '\0') {
        snprintf(error, error_size, "{\"error\":\"Machine name cannot be empty\"}");
        return 0; 
    }
    
    size_t len = strlen(name);
    
    if (len < 1 || len > 63) {
        snprintf(error, error_size, 
            "{\"error\":\"Machine name must be between 1 and 63 characters long\"}");
        return 0;
    }
    
    if (!isalnum((unsigned char)name[0])) {
        snprintf(error, error_size, 
            "{\"error\":\"Machine name must start with an alphanumeric character\"}");
        return 0;
    }
    
    if (!isalnum((unsigned char)name[len - 1])) {
        snprintf(error, error_size, 
            "{\"error\":\"Machine name must end with an alphanumeric character\"}");
        return 0;
    }
    
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)name[i];
        if (!isalnum(c) && c != '_' && c != '-') {
            snprintf(error, error_size, 
                "{\"error\":\"Invalid character '%c' in machine name. Only alphanumeric characters, underscores (_) and hyphens (-) are allowed\"}", 
                c);
            return 0;
        }
    }
    
    return 1; 
}

int isValidMachineName(const char* name) {
    char error[256];
    return validateMachineName(name, error, sizeof(error));
}
