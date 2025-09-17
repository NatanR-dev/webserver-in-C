#include "system.service.h"
#include "../shared/formats/json/json.h"
#include "../shared/platform/platform.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

void systemServiceInit(SystemService* service) {
    (void)service; 
}

void systemServiceCleanup(SystemService* service) {
    (void)service;
    free(service);
}

char* getMachineInfo(SystemService* service) {
    (void)service;
    
    char* result = malloc(1024);
    if (!result) return NULL;
    
    char machineId[256] = {0};
    char ip[46] = "127.0.0.1";
    
    if (platformGenerateMachineId(machineId, sizeof(machineId)) != 0) {
        strncpy(machineId, "unknown_id", sizeof(machineId) - 1);
    }
    
    if (platformGetLocalIp(ip, sizeof(ip)) != 0) {
        strncpy(ip, "127.0.0.1", sizeof(ip) - 1);
    }
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char dateTime[20];
    strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", tm_info);
    
    createJsonObject(result, 1024, 4,
        "status", "Started",
        "id", machineId,
        "ip", ip,
        "date", dateTime);
        
    return result;
}

char* getOsInfo(SystemService* service) {
    (void)service; 
    
    char* result = malloc(256);
    if (!result) return NULL;
    
    #ifdef PLATFORM_WINDOWS
        const char* osName = "Windows";
    #elif defined(PLATFORM_LINUX)
        const char* osName = "Linux";
    #elif defined(PLATFORM_MACOS)
        const char* osName = "macOS";
    #else
        const char* osName = "Unknown";
    #endif
    
    createJsonObject(result, 256, 1, "os", osName);
    return result;
}

char* getSystemInfo(SystemService* service) {
    (void)service; 
    
    char* result = malloc(4096);
    if (!result) return NULL;
    
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    char datetime[20];
    strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", timeinfo);

    #ifdef PLATFORM_WINDOWS
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        int arch = sysInfo.wProcessorArchitecture;
        const char* architectureName = "Unknown";
        const char* processorArchitecture = "Unknown";
        
        switch (arch) {
            case 0:
                architectureName = "x86";
                processorArchitecture = "x86";
                break;
            case 9:
                architectureName = "AMD64";
                processorArchitecture = "x64";
                break;
        }

        unsigned int numberOfLogicalProcessors = sysInfo.dwNumberOfProcessors;
        unsigned int numberOfCores = 0;
        
        DWORD bufferSize = 0;
        GetLogicalProcessorInformationEx(RelationProcessorCore, NULL, &bufferSize);
        if (bufferSize > 0) {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer = 
                (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)malloc(bufferSize);
            if (buffer) {
                if (GetLogicalProcessorInformationEx(RelationProcessorCore, buffer, &bufferSize)) {
                    for (DWORD i = 0; i < bufferSize; i += buffer->Size) {
                        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX current = 
                            (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((char*)buffer + i);
                        if (current->Relationship == RelationProcessorCore) {
                            numberOfCores++;
                        }
                    }
                }
                free(buffer);
            }
        }

        char coresStr[16], logicalStr[16];
        snprintf(coresStr, sizeof(coresStr), "%u", numberOfCores);
        snprintf(logicalStr, sizeof(logicalStr), "%u", numberOfLogicalProcessors);
        
        createJsonObject(result, 4096, 7,
            "os", "Windows",
            "arch", architectureName,
            "processorArchitecture", processorArchitecture,
            "numberOfCores", coresStr,
            "numberOfLogicalProcessors", logicalStr,
            "processorCount", logicalStr,
            "datetime", datetime);
            
    #elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
        struct utsname uname_data;
        uname(&uname_data);
        unsigned int num_cores = sysconf(_SC_NPROCESSORS_ONLN);

        const char* os_name = "Unknown";
        #ifdef PLATFORM_LINUX
            os_name = "Linux";
        #elif defined(PLATFORM_MACOS)
            os_name = "macOS";
        #endif

        char coresStr[16];
        snprintf(coresStr, sizeof(coresStr), "%u", num_cores);

        createJsonObject(result, 4096, 8,
            "os", os_name,
            "sysname", uname_data.sysname,
            "release", uname_data.release,
            "version", uname_data.version,
            "machine", uname_data.machine,
            "numberOfCores", coresStr,
            "numberOfLogicalProcessors", coresStr,
            "datetime", datetime);
    #else
        createJsonObject(result, 4096, 2,
            "os", "Unknown",
            "datetime", datetime);
    #endif

    return result;
}
