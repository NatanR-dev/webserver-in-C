// Common 
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

// Imports
#include "handlers.h"
#include "../utils/utils.h"
#include "../shared/formats/json/json.h"
#include "../shared/http/response.h"
#include "../shared/http/network.h"
#include "../shared/platform/platform.h"

void rootPathHandler(Server* server, PLATFORM_SOCKET clientConnection) {

    char routes[8192] = "[";
    size_t offset = 1; 
    
    for (int i = 0; i < server->routeCount; i++) {

        char fullUrl[2048];
        snprintf(fullUrl, sizeof(fullUrl), "http://localhost:8080%s", 
                server->routes[i].path);
        
        char routeObj[4096];
        createJsonObject(routeObj, sizeof(routeObj), 2,
            "path", server->routes[i].path,
            "link", fullUrl);
        
        if (i > 0) {
            offset += snprintf(routes + offset, sizeof(routes) - offset, ",\n        ");
        } else {
            offset += snprintf(routes + offset, sizeof(routes) - offset, "\n        ");
        }
        
        offset += snprintf(routes + offset, sizeof(routes) - offset, "%s", routeObj);
    }
    
    snprintf(routes + offset, sizeof(routes) - offset, "\n    ]");
    
    char json[16384];
    snprintf(json, sizeof(json),
        "{\n"
        "   \"message\": \"Welcome to low-level C API\",\n"
        "   \"version\": \"1.0\",\n"
        "   \"available_routes\": %s\n"
        "}",
        routes);
    
    sendJsonResponse(clientConnection, json);
}

void apiHandler(Server* server, PLATFORM_SOCKET clientConnection) {
    (void)server;
    char json[256];
    createJsonObject(json, sizeof(json), 2,
            "message", "Hello from my API!",
            "port", "8080");
    sendJsonResponse(clientConnection, json);
}

void machinesHandler(Server* server, PLATFORM_SOCKET clientConnection) {
    (void)server;  
    char machineId[33] = {0};
    char ip[46] = "127.0.0.1";  
    
    generateMachineId(machineId, sizeof(machineId));
    
    if (getLocalIP(ip, sizeof(ip)) != 0) {
        strncpy(ip, "127.0.0.1", sizeof(ip));
    }
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char dateTime[20];
    strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", tm_info);
    char json[1024];
    createJsonObject(json, sizeof(json), 4,
            "status", "Started",
            "id", machineId,
            "ip", ip,
            "date", dateTime);
    
    sendJsonResponse(clientConnection, json);
}

void osHandler(Server* server, PLATFORM_SOCKET clientConnection) {
    (void)server;  
    char osName[32] = "Unknown";
    
    #ifdef PLATFORM_WINDOWS
        strncpy(osName, "Windows", sizeof(osName));
    #elif defined(__linux__)
        strncpy(osName, "Linux", sizeof(osName));
    #elif defined(__APPLE__)
        strncpy(osName, "macOS", sizeof(osName));
    #endif
    
    char json[256];
    createJsonObject(json, sizeof(json), 1, "os", osName);
    sendJsonResponse(clientConnection, json);
}

void systemInfoHandler(Server* server, PLATFORM_SOCKET clientConnection) {
    (void)server;  
    char json[BUFFER_SIZE];

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
        
        createJsonObject(json, sizeof(json), 7,
            "os", "Windows",
            "arch", architectureName,
            "processorArchitecture", processorArchitecture,
            "numberOfCores", coresStr,
            "numberOfLogicalProcessors", logicalStr,
            "processorCount", logicalStr,  
            "datetime", datetime);
            
    #elif defined(__linux__) || defined(__APPLE__)
        struct utsname uname_data;
        uname(&uname_data);
        unsigned int num_cores = sysconf(_SC_NPROCESSORS_ONLN);

        const char* os_name = "Unknown";
        #ifdef __linux__
            os_name = "Linux";
        #elif defined(__APPLE__)
            os_name = "macOS";
        #endif

        char coresStr[16];
        snprintf(coresStr, sizeof(coresStr), "%u", num_cores);

        createJsonObject(json, sizeof(json), 8,
            "os", os_name,
            "sysname", uname_data.sysname,
            "release", uname_data.release,
            "version", uname_data.version,
            "machine", uname_data.machine,
            "numberOfCores", coresStr,
            "numberOfLogicalProcessors", coresStr,
            "datetime", datetime);
    #else
        createJsonObject(json, sizeof(json), 2,
            "os", "Unknown",
            "datetime", datetime);
    #endif

    sendJsonResponse(clientConnection, json);
}
