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

#define PORT 8080

int main() {
    init_sockets();

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    return 0;
}