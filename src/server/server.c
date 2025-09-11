// Common
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// WINDOWS
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
    #define close closesocket
    
    // Linking libs
    #ifdef _MSC_VER
        #pragma comment(lib, "ws2_32.lib")
    #endif
#else
    // UNIX-LIKE
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/types.h>
#endif

// Imports
#include "server.h"
#include "../routes/routes.h"

// Constants
#define PORT 8080
#define BUFFER_SIZE 4096

extern int handleRequest(Server* server, int clientConnection, char* request);

extern void handleClient(Server* server, int clientConnection);

void initSockets() {
    #ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    #endif
}

void cleanupSockets() {
    #ifdef _WIN32
    WSACleanup();
    #endif
}

void closeConnection(int clientConnection, char* response) {
    #ifdef _WIN32
    send(clientConnection, response, (int)strlen(response), 0);
    shutdown(clientConnection, SD_BOTH);
    closesocket(clientConnection);
    #else
    write(clientConnection, response, strlen(response));
    shutdown(clientConnection, SHUT_RDWR);
    close(clientConnection);
    #endif
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
        #ifdef _WIN32
        int clientSize = sizeof(clientAddress);
        #else
        socklen_t clientSize = sizeof(clientAddress);
        #endif
        int clientConnection = accept(server->socket, (struct sockaddr*)&clientAddress, &clientSize);
        if (clientConnection < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Connection received from %s\n", inet_ntoa(clientAddress.sin_addr));
        handleClient(server, clientConnection);
    }
}

void cleanupServer(Server* server) {
    for (int i = 0; i < server->routeCount; i++) {
        free(server->routes[i].path);
    }
}
