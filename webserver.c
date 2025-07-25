#include <stdio.h>
#include <unistd.h> 
#ifdef _WIN32
#include <windows.h> 
#endif

int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); 
    #endif

    for (int i = 1; ; i++) {
        printf("Receba-->\u26BD(%d)\n", i); 
        sleep(1); 
    }

    return 0;
}