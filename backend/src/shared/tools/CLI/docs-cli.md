# ❯❯ Generating a new module ❒

## ||> You're gonna need a terminal with shell support <||

### Windows:
```bash
// You can use Git bash 

// Add a variable to your shell
export CLI="./backend/src/shared/tools/CLI/generate.module.sh"

$CLI <moduleName>
```

### MacOS:
```bash
// Use the zsh
$CLI <moduleName>
```

### Linux:
```bash
// You can use a native terminal like GNOME Terminal, Konsole, etc
$CLI <moduleName>
```

## ||> After that, you need to register the module in main.c <||
For example considering that you have created users module:

➀ Include the header
```c
#include "users/users.module.h"
```

➁ Initialize the module
```c
usersModuleInit(&server);
```

➂ Cleanup the module
```c
usersModuleCleanup(&server);
```

Main.c example:
```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "shared/platform/platform.h"

#include "shared/http/server/server.h"

// Modules
#include "root/root.module.h"
#include "system/system.module.h"
#include "users/users.module.h" // <--- Include the header

int main()
{
    initSockets();
    
    Server server;
    if (initializeServer(&server, 8080) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }

    rootModuleInit(&server);
    systemModuleInit(&server);
    usersModuleInit(&server); // <--- Initialize the module

    printf("Server started on port %d, waiting for connections...\n", server.port);
    
    while (1) {
        PLATFORM_SOCKET clientConnection = acceptConnection(&server);
        if (clientConnection != INVALID_PLATFORM_SOCKET) {
            handleClient(&server, clientConnection);
        } else {
            char errorMsg[256];
            platformGetErrorString(platformGetLastError(), errorMsg, sizeof(errorMsg));
            fprintf(stderr, "accept failed: %s\n", errorMsg);
        }
    }

    systemModuleCleanup(&server);
    rootModuleCleanup(&server);
    usersModuleCleanup(&server); // <--- Cleanup the module
    cleanupServer(&server);
    cleanupSockets();
    return 0;
}
```

➃ Build the project:
```bash
make clean && make && make run
```
Output:
```bash
"Starting webserver.exe..."
LOG: System module initialized
[INFO] users module initialized
Server started on port 8080, waiting for connections...
```

➄ Test the module, use [Request.http](../Request.http) on root directory:
```bash
GET http://localhost:8080/users
```
Expected output:
```json
{"message": "Hello from users module!", "port": "8080"}
```

## ☓ Troubleshooting

- Script fails: Check for [ERROR] messages (e.g., invalid module name or existing directory). Run Git Bash as administrator on Windows.

- Module not working: Ensure <moduleName>.module.c is compiled in the Makefile and <moduleName>ModuleInit is called in main.c.

- HTTP 500 error: Check server logs for issues in <moduleName>.service.c.