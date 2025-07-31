#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <winsock2.h>

typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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

void send_json_response(int clientConnection) {
    char json[BUFFER_SIZE];
    snprintf(json, sizeof(json), "{\"message\": \"Hello from my API!\", \"port\": %d}", PORT);
    char response[BUFFER_SIZE * 2];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %d\r\nConnection: keep-alive\r\n\r\n%s", (int)strlen(json), json);
    send(clientConnection, response, strlen(response), 0);
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
    char json[BUFFER_SIZE * 2];
    int offset = snprintf(json, sizeof(json), "{\"message\": \"Welcome to API\", \"version\": \"1.0\", \"available_routes\": [");
    for (int i = 0; i < server->routeCount; i++) {
        char escaped_path[512];
        json_escape_string(server->routes[i].path, escaped_path, sizeof(escaped_path));
        offset += snprintf(json + offset, sizeof(json) - offset, "{\"path\": \"%s\", \"link\": \"http://localhost:8080%s\"}", escaped_path, escaped_path);
        if (i < server->routeCount - 1) {
            offset += snprintf(json + offset, sizeof(json) - offset, ", ");
        }
    }
    snprintf(json + offset, sizeof(json) - offset, "]}");
    char response[BUFFER_SIZE * 3];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s", (int)strlen(json), json);
    send(clientConnection, response, strlen(response), 0);
}

void apiHandler(Server* server, int clientConnection) {
    send_json_response(clientConnection);
}

void wrapperHandler(Server* server, int clientConnection, RouteHandler handler) {
    handler(server, clientConnection);
}

int handleRequest(Server* server, int clientConnection, char* request) {
    char method[10], path[256], version[10];
    if (sscanf(request, "%9s %255s %9s", method, path, version) != 3) {
        printf("Invalid request format\n");
        return 0;
    }
    
    if (strcmp(method, "GET") != 0) {
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\nContent-Length: 18\r\nConnection: close\r\n\r\nMethod Not Allowed");
        send(clientConnection, response, strlen(response), 0);
        return 0;
    }
    
    printf("Path: %s\n", path);

    for (int i = 0; i < server->routeCount; i++) {
        if (strcmp(server->routes[i].path, path) == 0) {
            printf("Found route for path: %s\n", path);
            wrapperHandler(server, clientConnection, server->routes[i].handler);
            return 1;
        }
    }

    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 9\r\nConnection: close\r\n\r\nNot Found");
    send(clientConnection, response, strlen(response), 0);
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
            if (handleRequest(server, clientConnection, request) == 0) {
                close_connection(clientConnection);
                printf("Connection closed\n");
                break;
            }
        } else if (bytesReceived == 0) {
            printf("Client disconnected\n");
            break;
        } else {
            printf("Error receiving request: %d\n", bytesReceived);
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

    startServer(&server);

    cleanupServer(&server);
    cleanup_sockets();
    return 0;
}