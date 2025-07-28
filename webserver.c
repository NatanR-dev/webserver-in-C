#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>

typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#endif

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_ROUTES 10

typedef void (*RouteHandler)(int);

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
    server->routes[server->routeCount].path = strdup(path);
    server->routes[server->routeCount].handler = handler;
    server->routeCount++;
}

void send_json_response(int clientConnection) {
    char json[BUFFER_SIZE];
    sprintf(json, "{\"message\": \"Hello from my API!\", \"port\": %d}", PORT);
    char response[BUFFER_SIZE * 2];
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %d\r\nConnection: keep-alive\r\n\r\n%s", strlen(json), json);
    send(clientConnection, response, strlen(response), 0);
}

void rootPathHandler(int clientConnection) {
    char json[BUFFER_SIZE];
    sprintf(json, "{\"message\": \"Welcome to API\", \"version\": \"1.0.0\", \"available_routes\": [\"/api\"]}");
    char response[BUFFER_SIZE * 2];
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s", strlen(json), json);
    send(clientConnection, response, strlen(response), 0);
}

void apiHandler(int clientConnection) {
    send_json_response(clientConnection);
}

int handleRequest(Server* server, int clientConnection, char* request) {
    printf("Received request: %s\n", request);
    char* path = strtok(request, " ");
    if (path == NULL) {
        printf("Invalid request\n");
        return 0;
    }
    path = strtok(NULL, " ");
    if (path == NULL) {
        printf("Invalid request\n");
        return 0;
    }
    printf("Path: %s\n", path);

    for (int i = 0; i < server->routeCount; i++) {
        if (strcmp(server->routes[i].path, path) == 0) {
            printf("Found route for path: %s\n", path);
            server->routes[i].handler(clientConnection);
            return 1;
        }
    }

    char response[BUFFER_SIZE];
    sprintf(response, "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 9\r\nConnection: close\r\n\r\nNot Found");
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
    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);
    
    bind(server->socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(server->socket, 3);

    printf("Server started on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in clientAddress = {0};
        socklen_t clientSize = sizeof(clientAddress);
        int clientConnection = accept(server->socket, (struct sockaddr*)&clientAddress, &clientSize);
        printf("Connection received from %s\n", inet_ntoa(clientAddress.sin_addr));
        handleClient(server, clientConnection);
    }
}

int main() {
    init_sockets();

    Server server;
    server.routeCount = 0;

    addRoute(&server, "/", rootPathHandler);

    addRoute(&server, "/api", apiHandler);

    startServer(&server);

    cleanup_sockets();
    return 0;
}