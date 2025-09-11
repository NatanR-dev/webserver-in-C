#include "../platform.h"
#include <stdio.h>

int platformGetLastError(void) {
    return WSAGetLastError();
}

int platformGetErrorString(int error, char* buffer, size_t bufferSize) {
    if (!buffer || bufferSize == 0) {
        return -1;
    }
    
    DWORD result = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        (DWORD)error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)buffer,
        (DWORD)bufferSize,
        NULL);
        
    return (result > 0) ? 0 : -1;
}

int platformNetworkingInit(void) {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        return -1;
    }
    
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        return -1;
    }
    
    return 0;
}

void platformNetworkingCleanup(void) {
    WSACleanup();
}

int platformGetHostname(char* buffer, size_t size) {
    if (!buffer || size == 0) {
        return -1;
    }
    
    DWORD result = GetComputerNameA(buffer, (LPDWORD)&size);
    return (result != 0) ? 0 : -1;
}
