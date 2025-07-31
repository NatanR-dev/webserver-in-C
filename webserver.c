#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>

typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <time.h>
#if defined(__linux__) || defined(__APPLE__)
#include <sys/utsname.h>
#endif
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_ROUTES 10

typedef struct Server Server;
typedef void (*RouteHandler)(Server*, int);

typedef struct Route {
    char* path;
    RouteHandler handler;
} Route;

typedef struct Server {
    int socket;
    Route routes[MAX_ROUTES];
    int routeCount;
} Server;

void init_sockets() {
    #ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    #endif
}

void cleanup_sockets() {
    #ifdef _WIN32
    WSACleanup();
    #endif
}

void close_connection(int clientConnection) {
    #ifdef _WIN32
    shutdown(clientConnection, SD_BOTH);
    closesocket(clientConnection);
    #else
    shutdown(clientConnection, SHUT_RDWR);
    close(clientConnection);
    #endif
}

void addRoute(Server* server, char* path, RouteHandler handler) {
    if (server->routeCount >= MAX_ROUTES) {
        fprintf(stderr, "Error: Maximum number of routes (%d) exceeded\n", MAX_ROUTES);
        return;
    }
    server->routes[server->routeCount].path = strdup(path);
    server->routes[server->routeCount].handler = handler;
    server->routeCount++;
}

void cleanupServer(Server* server) {
    for (int i = 0; i < server->routeCount; i++) {
        free(server->routes[i].path);
    }
}

void send_http_response(int clientConnection, int status_code, const char* status_message, const char* content_type, const char* body, const char* connection) {
    int required_size = snprintf(NULL, 0, "HTTP/1.1 %d %s\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: %s\r\n\r\n%s", 
                                status_code, status_message, content_type, (int)strlen(body), connection, body) + 1;
    char* response = malloc(required_size);
    if (!response) return;
    snprintf(response, required_size, "HTTP/1.1 %d %s\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: %s\r\n\r\n%s", status_code, status_message, content_type, (int)strlen(body), connection, body);
    send(clientConnection, response, strlen(response), 0);
    free(response);
}

void send_json_response(int clientConnection, const char* json) {
    send_http_response(clientConnection, 200, "OK", "application/json", json, "keep-alive");
}

void json_escape_string(const char* input, char* output, size_t output_size) {
    size_t j = 0;
    for (size_t i = 0; input[i] && j < output_size - 2; i++) {
        if (input[i] == '"' || input[i] == '\\') {
            if (j < output_size - 3) {
                output[j++] = '\\';
                output[j++] = input[i];
            }
        } else {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
}

void rootPathHandler(Server* server, int clientConnection) {
    int size = snprintf(NULL, 0, "{\"message\": \"Welcome to API\", \"version\": \"1.0\", \"available_routes\": [");
    for (int i = 0; i < server->routeCount; i++) {
        char escaped_path[1024];
        json_escape_string(server->routes[i].path, escaped_path, sizeof(escaped_path));
        size += snprintf(NULL, 0, "{\"path\": \"%s\", \"link\": \"http://localhost:8080%s\"}", escaped_path, escaped_path);
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
        char escaped_path[1024];
        json_escape_string(server->routes[i].path, escaped_path, sizeof(escaped_path));
        offset += snprintf(json + offset, size - offset, "{\"path\": \"%s\", \"link\": \"http://localhost:8080%s\"}", escaped_path, escaped_path);
        if (i < server->routeCount - 1) {
            offset += snprintf(json + offset, size - offset, ", ");
        }
    }
    snprintf(json + offset, size - offset, "]}");

    send_json_response(clientConnection, json);
    free(json);
}

void apiHandler(Server* server, int clientConnection) {
    send_json_response(clientConnection, "{\"message\": \"Hello from my API!\", \"port\": 8080}");
}

void osHandler(Server* server, int clientConnection) {
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

    send_json_response(clientConnection, json);
}

void systemInfoHandler(Server* server, int clientConnection) {
    char json[BUFFER_SIZE];

    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    char datetime[20];
    strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", timeinfo);

    #ifdef _WIN32
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        int arch = sysInfo.wProcessorArchitecture;
        char* processorArchitecture;
        switch (arch) {
            case 0:
                processorArchitecture = "x86";
                break;
            case 9:
                processorArchitecture = "x64";
                break;
            default:
                processorArchitecture = "Unknown";
                break;
        }

        unsigned int numberOfLogicalProcessors = sysInfo.dwNumberOfProcessors;

        DWORD bufferSize = 0;
        GetLogicalProcessorInformationEx(RelationProcessorCore, NULL, &bufferSize);
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)malloc(bufferSize);
        GetLogicalProcessorInformationEx(RelationProcessorCore, buffer, &bufferSize);
        unsigned int numberOfCores = 0;
        for (DWORD i = 0; i < bufferSize; i += buffer->Size) {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX current = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((char*)buffer + i);
            if (current->Relationship == RelationProcessorCore) {
                numberOfCores++;
            }
        }
        free(buffer);

        snprintf(json, sizeof(json), "{\"os\": \"Windows\", \"arch\": \"%d\", \"processorArchitecture\": \"%s\", \"numberOfCores\": \"%u\", \"numberOfLogicalProcessors\": \"%u\", \"processorCount\": \"%u\", \"datetime\": \"%s\"}", 
            arch, processorArchitecture, numberOfCores, numberOfLogicalProcessors, numberOfLogicalProcessors, datetime);
    #elif defined(__linux__)
        struct utsname uname_data;
        uname(&uname_data);
        unsigned int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
        snprintf(json, sizeof(json), "{\"os\": \"Linux\", \"sysname\": \"%s\", \"release\": \"%s\", \"version\": \"%s\", \"machine\": \"%s\", \"numberOfCores\": \"%u\", \"numberOfLogicalProcessors\": \"%u\", \"datetime\": \"%s\"}", 
            uname_data.sysname, uname_data.release, uname_data.version, uname_data.machine, num_cores, num_cores, datetime);
    #elif defined(__APPLE__)
        struct utsname uname_data;
        uname(&uname_data);
        unsigned int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
        snprintf(json, sizeof(json), "{\"os\": \"macOS\", \"sysname\": \"%s\", \"release\": \"%s\", \"version\": \"%s\", \"machine\": \"%s\", \"numberOfCores\": \"%u\", \"numberOfLogicalProcessors\": \"%u\", \"datetime\": \"%s\"}", 
            uname_data.sysname, uname_data.release, uname_data.version, uname_data.machine, num_cores, num_cores, datetime);
    #else
        snprintf(json, sizeof(json), "{\"os\": \"Unknown\", \"datetime\": \"%s\"}", datetime);
    #endif

    send_json_response(clientConnection, json);
}

void send_error_response(int clientConnection, int status_code, const char* status_message, const char* body) {
    send_http_response(clientConnection, status_code, status_message, "text/plain", body, "close");
}

int handleRequest(Server* server, int clientConnection, char* request) {
    char method[10], path[256], version[10];
    if (sscanf(request, "%9s %255s %9s", method, path, version) != 3) {
        send_error_response(clientConnection, 400, "Bad Request", "Bad Request");
        return 0;
    }

    if (strcmp(method, "GET") != 0) {
        send_error_response(clientConnection, 405, "Method Not Allowed", "Method Not Allowed");
        return 0;
    }

    for (int i = 0; i < server->routeCount; i++) {
        if (strcmp(server->routes[i].path, path) == 0) {
            printf("Found route for path: %s\n", path);
            server->routes[i].handler(server, clientConnection);
            return 1;
        }
    }

    send_error_response(clientConnection, 404, "Not Found", "Not Found");
    printf("Sent 404 response\n");
    return 0;
}

void handleClient(Server* server, int clientConnection) {
    while (1) {
        char request[BUFFER_SIZE];
        int bytesReceived = recv(clientConnection, request, BUFFER_SIZE, 0);
        if (bytesReceived > 0) {
            request[bytesReceived] = '\0';
            printf("Received request: %s\n", request);
            int handled = handleRequest(server, clientConnection, request);
            if (!handled) {
                close_connection(clientConnection);
                printf("Connection closed\n");
                break;
            }
        } else if (bytesReceived == 0) {
            printf("Client disconnected\n");
            close_connection(clientConnection);
            break;
        } else {
            printf("Error receiving request: %d\n", bytesReceived);
            close_connection(clientConnection);
            break;
        }
    }
}

void startServer(Server* server) {
    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);
    
    if (bind(server->socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Bind failed");
        close(server->socket);
        exit(1);
    }
    
    if (listen(server->socket, 3) < 0) {
        perror("Listen failed");
        close(server->socket);
        exit(1);
    }

    printf("Server started on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in clientAddress = {0};
        socklen_t clientSize = sizeof(clientAddress);
        int clientConnection = accept(server->socket, (struct sockaddr*)&clientAddress, &clientSize);
        if (clientConnection < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Connection received from %s\n", inet_ntoa(clientAddress.sin_addr));
        handleClient(server, clientConnection);
    }
}

int main() {
    init_sockets();

    Server server = {0};

    addRoute(&server, "/", rootPathHandler);
    addRoute(&server, "/api", apiHandler);
    addRoute(&server, "/os", osHandler);
    addRoute(&server, "/sys", systemInfoHandler);

    startServer(&server);

    cleanupServer(&server);
    cleanup_sockets();
    return 0;
}