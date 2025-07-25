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

int main() {

    return 0;
}