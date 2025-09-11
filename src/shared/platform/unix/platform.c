#include "../platform.h"
#include <string.h>
#include <errno.h>

int platformGetLastError(void) {
    return errno;
}

int platformGetErrorString(int error, char* buffer, size_t bufferSize) {
    if (!buffer || bufferSize == 0) {
        return -1;
    }
    
    const char* error_str = strerror(error);
    if (!error_str) {
        return -1;
    }
    
    size_t len = strlen(error_str);
    if (len >= bufferSize) {
        len = bufferSize - 1;
    }
    
    strncpy(buffer, error_str, len);
    buffer[len] = '\0';
    
    return 0;
}

int platformGetHostname(char* buffer, size_t size) {
    if (!buffer || size == 0) {
        return -1;
    }
    
    return gethostname(buffer, size);
}
