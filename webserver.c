#include <stdio.h>
#ifdef _WIN32
#include <winsock2.h>

typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define PORT 8080
#define BUFFER_SIZE 1024

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

void send_html_response(int clientConnection) {
    char* html = "<html><body><h1>Hello, World!</h1></body></html>";
    char response[BUFFER_SIZE];
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s", strlen(html), html);
    send(clientConnection, response, strlen(response), 0);
}

int main() {
    init_sockets();

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
        
        int bytesReceived;
        do {
            char request[BUFFER_SIZE];
            bytesReceived = recv(clientConnection, request, BUFFER_SIZE, 0);
            if (bytesReceived > 0) {
                send_html_response(clientConnection);
            }
        } while (bytesReceived > 0);
        
        close_connection(clientConnection);
    }

    cleanup_sockets();
    return 0;
}