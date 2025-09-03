#include "handlers.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <sys/utsname.h>
#include <unistd.h>
#endif

void rootPathHandler(Server* server, int clientConnection) {
    int size = snprintf(NULL, 0, "{\"message\": \"Welcome to API\", \"version\": \"1.0\", \"available_routes\": [");
    for (int i = 0; i < server->routeCount; i++) {
        char escapedPath[1024];
        jsonEscapeString(server->routes[i].path, escapedPath, sizeof(escapedPath));
        size += snprintf(NULL, 0, "{\"path\": \"%s\", \"link\": \"http://localhost:8080%s\"}", escapedPath, escapedPath);
        if (i < server->routeCount - 1) {
            size += snprintf(NULL, 0, ", ");
        }
    }
    size += snprintf(NULL, 0, "]}");
    size++; 

    char* json = malloc(size);
    if (json == NULL) {
        return;
    }

    int offset = snprintf(json, size, "{\"message\": \"Welcome to API\", \"version\": \"1.0\", \"available_routes\": [");
    for (int i = 0; i < server->routeCount; i++) {
        char escapedPath[1024];
        jsonEscapeString(server->routes[i].path, escapedPath, sizeof(escapedPath));
        offset += snprintf(json + offset, size - offset, "{\"path\": \"%s\", \"link\": \"http://localhost:8080%s\"}", escapedPath, escapedPath);
        if (i < server->routeCount - 1) {
            offset += snprintf(json + offset, size - offset, ", ");
        }
    }
    snprintf(json + offset, size - offset, "]}");

    sendJsonResponse(clientConnection, json);
    free(json);
}

void apiHandler(Server* server, int clientConnection) {
    (void)server;  
    sendJsonResponse(clientConnection, "{\"message\": \"Hello from my API!\", \"port\": 8080}");
}

void osHandler(Server* server, int clientConnection) {
    (void)server;  
    char json[BUFFER_SIZE];

    #ifdef _WIN32
        snprintf(json, sizeof(json), "{\"os\": \"Windows\"}");
    #elif defined(__linux__)
        snprintf(json, sizeof(json), "{\"os\": \"Linux\"}");
    #elif defined(__APPLE__)
        snprintf(json, sizeof(json), "{\"os\": \"macOS\"}");
    #else
        snprintf(json, sizeof(json), "{\"os\": \"Unknown\"}");
    #endif

    sendJsonResponse(clientConnection, json);
}

void systemInfoHandler(Server* server, int clientConnection) {
    (void)server;  
    char json[BUFFER_SIZE];

    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    char datetime[20];
    strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", timeinfo);

    #ifdef _WIN32
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        int arch = sysInfo.wProcessorArchitecture;
        char* architectureName;
        char* processorArchitecture;
        
        switch (arch) {
            case 0:
                architectureName = "x86";
                processorArchitecture = "x86";
                break;
            case 9:
                architectureName = "AMD64";
                processorArchitecture = "x64";
                break;
            default:
                architectureName = "Unknown";
                processorArchitecture = "Unknown";
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

        char escapedProcessorArchitecture[256];
        jsonEscapeString(processorArchitecture, escapedProcessorArchitecture, sizeof(escapedProcessorArchitecture));
        
        char escapedArchitectureName[256];
        jsonEscapeString(architectureName, escapedArchitectureName, sizeof(escapedArchitectureName));
        
        snprintf(json, sizeof(json), 
            "{\"os\": \"Windows\", "
            "\"arch\": \"%s\", "
            "\"processorArchitecture\": \"%s\", "
            "\"numberOfCores\": \"%u\", "
            "\"numberOfLogicalProcessors\": \"%u\", "
            "\"processorCount\": \"%u\", "
            "\"datetime\": \"%s\"}", 
            escapedArchitectureName, 
            escapedProcessorArchitecture, 
            numberOfCores, 
            numberOfLogicalProcessors, 
            numberOfLogicalProcessors, 
            datetime);
            
    #elif defined(__linux__) || defined(__APPLE__)
        struct utsname uname_data;
        uname(&uname_data);
        unsigned int num_cores = sysconf(_SC_NPROCESSORS_ONLN);

        char escapedSysname[512], escapedRelease[512], escapedVersion[512], escapedMachine[512];
        jsonEscapeString(uname_data.sysname, escapedSysname, sizeof(escapedSysname));
        jsonEscapeString(uname_data.release, escapedRelease, sizeof(escapedRelease));
        jsonEscapeString(uname_data.version, escapedVersion, sizeof(escapedVersion));
        jsonEscapeString(uname_data.machine, escapedMachine, sizeof(escapedMachine));

        const char* os_name = 
        #ifdef __linux__
            "Linux";
        #elif defined(__APPLE__)
            "macOS";
        #endif

        snprintf(json, sizeof(json), 
            "{\"os\": \"%s\", "
            "\"sysname\": \"%s\", "
            "\"release\": \"%s\", "
            "\"version\": \"%s\", "
            "\"machine\": \"%s\", "
            "\"numberOfCores\": \"%u\", "
            "\"numberOfLogicalProcessors\": \"%u\", "
            "\"datetime\": \"%s\"}", 
            os_name,
            escapedSysname, 
            escapedRelease, 
            escapedVersion, 
            escapedMachine, 
            num_cores, 
            num_cores, 
            datetime);
    #else
        snprintf(json, sizeof(json), "{\"os\": \"Unknown\", \"datetime\": \"%s\"}", datetime);
    #endif

    sendJsonResponse(clientConnection, json);
}
