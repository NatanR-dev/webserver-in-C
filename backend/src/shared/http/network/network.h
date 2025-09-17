#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <stddef.h>

// Platform 
#include "../../platform/platform.h"

/**
 * @brief Gets the local IP address
 * @param ip Buffer to store the IP address
 * @param ipSize Size of the buffer
 * @return 0 on success, non-zero on error
 */
int getLocalIP(char* ip, size_t ipSize);

#endif // NETWORK_UTILS_H
