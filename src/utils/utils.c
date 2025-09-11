#include "utils.h"

// Common
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    // WINDOWS
    #include <windows.h>
    #include <iphlpapi.h>
    #include <intrin.h>
    #ifdef _MSC_VER
        #pragma comment(lib, "iphlpapi.lib")
    #endif
#else
    // UNIX-LIKE
    #include <unistd.h>
    #include <sys/utsname.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <errno.h>
#endif

void generateMachineId(char* id, size_t idSize) {
    if (id == NULL || idSize == 0) {
        return;
    }

    unsigned char hash[16] = {0};
    size_t hashSize = 0;

    #ifdef _WIN32
        // Windows - Serial number
        char volumeName[MAX_PATH + 1] = {0};
        char fileSystemName[MAX_PATH + 1] = {0};
        DWORD serialNumber = 0;
        DWORD maxComponentLen = 0;
        DWORD fileSystemFlags = 0;

        if (GetVolumeInformationA(
            "C:\\",
            volumeName,
            sizeof(volumeName),
            &serialNumber,
            &maxComponentLen,
            &fileSystemFlags,
            fileSystemName,
            sizeof(fileSystemName))) {
            
            // Volume serial number - hash
            memcpy(hash, &serialNumber, sizeof(serialNumber));
            hashSize = sizeof(serialNumber);
        }
    #else
        // Unix-like - Uses machine-id, otherwise  hostname 
        int fd = open("/etc/machine-id", O_RDONLY);
        if (fd >= 0) {
            ssize_t bytesRead = read(fd, hash, sizeof(hash));
            if (bytesRead > 0) {
                hashSize = bytesRead;
            }
            close(fd);
        }

        if (hashSize == 0) {
            // Fallback to hostname
            struct utsname name;
            if (uname(&name) == 0) {
                strncpy((char*)hash, name.nodename, sizeof(hash) - 1);
                hashSize = strlen(name.nodename);
            }
        }
    #endif

    //!unique ID, random
    if (hashSize == 0) {
        srand((unsigned int)time(NULL));
        for (size_t i = 0; i < sizeof(hash); i++) {
            hash[i] = (unsigned char)(rand() % 256);
        }
        hashSize = sizeof(hash);
    }

    // Convert hash to hex string
    const char* hexChars = "0123456789abcdef";
    size_t outputSize = (hashSize * 2) + 1;
    if (outputSize > idSize) {
        outputSize = idSize;
    }

    for (size_t i = 0, j = 0; i < hashSize && j + 1 < outputSize; i++) {
        id[j++] = hexChars[(hash[i] >> 4) & 0x0F];
        if (j + 1 < outputSize) {
            id[j++] = hexChars[hash[i] & 0x0F];
        }
    }
    
    // Null-terminate the string
    if (outputSize > 0) {
        id[outputSize - 1] = '\0';
    }
}
