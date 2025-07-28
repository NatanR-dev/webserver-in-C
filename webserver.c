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

Route routes[MAX_ROUTES];
int routeCount = 0;

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
    closesocket(clientConnection);
    #else
    close(clientConnection);
    #endif
}

void addRoute(char* path, RouteHandler handler) {
    routes[routeCount].path = strdup(path);
    routes[routeCount].handler = handler;
    routeCount++;
}

void send_json_response(int clientConnection) {
    char json[BUFFER_SIZE];
    sprintf(json, "{\"message\": \"Hello from my API!\", \"port\": %d}", PORT);
    char response[BUFFER_SIZE * 2];
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %d\r\nConnection: keep-alive\r\n\r\n%s", strlen(json), json);
    send(clientConnection, response, strlen(response), 0);
}

void apiHandler(int clientConnection) {
    send_json_response(clientConnection);
}

void handleRequest(int clientConnection, char* request) {
    printf("Received request: %s\n", request);
    char* path = strtok(request, " ");
    if (path == NULL) {
        printf("Invalid request\n");
        return;
    }
    path = strtok(NULL, " ");
    if (path == NULL) {
        printf("Invalid request\n");
        return;
    }
    printf("Path: %s\n", path);

    for (int i = 0; i < routeCount; i++) {
        if (strcmp(routes[i].path, path) == 0) {
            printf("Found route for path: %s\n", path);
            routes[i].handler(clientConnection);
            return;
        }
    }

    char response[BUFFER_SIZE];
    sprintf(response, "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 9\r\nConnection: keep-alive\r\n\r\nNot Found");
    send(clientConnection, response, strlen(response), 0);
    printf("Sent 404 response\n");
}

int main() {
    init_sockets();

    addRoute("/api", apiHandler);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);
    
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 3);

    printf("Server started on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in clientAddress = {0};
        socklen_t clientSize = sizeof(clientAddress);
        int clientConnection = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientSize);
        printf("Connection received from %s\n", inet_ntoa(clientAddress.sin_addr));
    
        while (1) {
            char request[BUFFER_SIZE];
            int bytesReceived = recv(clientConnection, request, BUFFER_SIZE, 0);
            if (bytesReceived > 0) {
                printf("Received request: %s\n", request);
                handleRequest(clientConnection, request);
            } else if (bytesReceived == 0) {
                printf("Client disconnected\n");
                break;
            } else {
                printf("Error receiving request: %d\n", bytesReceived);
                break;
            }
        }
    
        close_connection(clientConnection);
        printf("Connection closed\n");
    }

    cleanup_sockets();
    return 0;
}