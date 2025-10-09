@echo off
setlocal enabledelayedexpansion

:: Check if a module name was provided
if "%~1"=="" (
    echo [ERROR] Usage: create-module <module_name>
    echo [ERROR] Example: create-module users
    exit /b 1
)

set "MODULE_NAME=%~1"

:: Validate module name
echo %MODULE_NAME%|findstr /r "^[a-z][a-z0-9_]*$" >nul
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Invalid module name. Use only lowercase letters, numbers, and underscores.
    exit /b 1
)

:: Paths
set "SCRIPT_DIR=%~dp0"
set "TARGET_DIR=%SCRIPT_DIR%..\backend\src\%MODULE_NAME%"

:: Remove extra quotes
set "TARGET_DIR=%TARGET_DIR:"=%"

:: Check if directory already exists
if exist "%TARGET_DIR%" (
    echo [ERROR] Module '%MODULE_NAME%' already exists at %TARGET_DIR%
    exit /b 1
)

:: Create new module directory
echo [INFO] Creating directory: %TARGET_DIR%
mkdir "%TARGET_DIR%"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to create directory %TARGET_DIR%
    exit /b 1
)

echo [INFO] Creating new module: %MODULE_NAME%

:: Create files with boilerplate content
echo [INFO] Creating %MODULE_NAME%.controller.h
(
    echo #ifndef %MODULE_NAME%_CONTROLLER_H
    echo #define %MODULE_NAME%_CONTROLLER_H
    echo.
    echo #include "../../shared/http/server/server.h"
    echo #include "../../shared/http/router/routes.h"
    echo.
    echo typedef struct %MODULE_NAME%Service %MODULE_NAME%Service;
    echo.
    echo /// @brief Gets the controller routes
    echo /// @param count Pointer to store the number of routes
    echo /// @return Array of route configurations
    echo const RouteConfig* get%MODULE_NAME%Routes(int* count^);
    echo.
    echo /// @brief Gets the service instance
    echo /// @return Pointer to the service
    echo %MODULE_NAME%Service* get%MODULE_NAME%Service(void^);
    echo.
    echo /// @brief Handles GET request for /%MODULE_NAME%
    echo /// @param server Pointer to the server
    echo /// @param clientConnection Client connection
    echo void handleGet%MODULE_NAME%(void* server, void* clientConnection^);
    echo.
    echo #endif // %MODULE_NAME%_CONTROLLER_H
) > "%TARGET_DIR%\%MODULE_NAME%.controller.h"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to create %MODULE_NAME%.controller.h
    exit /b 1
)

echo [INFO] Creating %MODULE_NAME%.controller.c
(
    echo #include "%MODULE_NAME%.controller.h"
    echo #include "%MODULE_NAME%.service.h"
    echo.
    echo #include "../../shared/http/response/response.h"
    echo #include "../../shared/http/router/route.decorators.h"
    echo #include "../../shared/http/server/server.h"
    echo.
    echo #include <stdlib.h>
    echo #include <string.h>
    echo.
    echo static %MODULE_NAME%Service* service = NULL;
    echo.
    echo static const RouteConfig %MODULE_NAME%Routes[] = {
    echo     GET("/%MODULE_NAME%", handleGet%MODULE_NAME%^)
    echo };
    echo.
    echo const RouteConfig* get%MODULE_NAME%Routes(int* count^) {
    echo     if (count^) {
    echo         *count = sizeof(%MODULE_NAME%Routes^) / sizeof(RouteConfig^);
    echo     }
    echo     return %MODULE_NAME%Routes;
    echo }
    echo.
    echo %MODULE_NAME%Service* get%MODULE_NAME%Service(void^) {
    echo     return service;
    echo }
    echo.
    echo void handleGet%MODULE_NAME%(void* serverPtr, void* clientConnectionPtr^) {
    echo     Server* server = (Server*^)serverPtr;
    echo     PLATFORM_SOCKET clientConnection = (PLATFORM_SOCKET^)(uintptr_t^)clientConnectionPtr;
    echo.
    echo     char* response = get%MODULE_NAME%Info(get%MODULE_NAME%Service(^), server^);
    echo     if (response^) {
    echo         sendJsonResponse(clientConnection, response^);
    echo         free(response^);
    echo     } else {
    echo         sendErrorResponse(clientConnection, 500, "Internal Server Error", "An error occurred while processing your request."^);
    echo     }
    echo }
) > "%TARGET_DIR%\%MODULE_NAME%.controller.c"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to create %MODULE_NAME%.controller.c
    exit /b 1
)

echo [INFO] Creating %MODULE_NAME%.service.h
(
    echo #ifndef %MODULE_NAME%_SERVICE_H
    echo #define %MODULE_NAME%_SERVICE_H
    echo.
    echo #include "../../shared/http/server/server.h"
    echo.
    echo typedef struct %MODULE_NAME%Service {
    echo     // Add necessary service fields here
    echo } %MODULE_NAME%Service;
    echo.
    echo /// @brief Initializes the service
    echo /// @param service Pointer to the service
    echo void %MODULE_NAME%ServiceInit(%MODULE_NAME%Service* service^);
    echo.
    echo /// @brief Cleans up service resources
    echo /// @param service Pointer to the service
    echo void %MODULE_NAME%ServiceCleanup(%MODULE_NAME%Service* service^);
    echo.
    echo /// @brief Gets module information for JSON response
    echo /// @param service Pointer to the service
    echo /// @param server Pointer to the server
    echo /// @return Dynamically allocated JSON string (must be freed by caller^)
    echo char* get%MODULE_NAME%Info(%MODULE_NAME%Service* service, Server* server^);
    echo.
    echo #endif // %MODULE_NAME%_SERVICE_H
) > "%TARGET_DIR%\%MODULE_NAME%.service.h"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to create %MODULE_NAME%.service.h
    exit /b 1
)

echo [INFO] Creating %MODULE_NAME%.service.c
(
    echo #include "%MODULE_NAME%.service.h"
    echo.
    echo #include "../../shared/formats/json/json.h"
    echo #include <stdlib.h>
    echo #include <string.h>
    echo.
    echo void %MODULE_NAME%ServiceInit(%MODULE_NAME%Service* service^) {
    echo     (void^)service;
    echo }
    echo.
    echo void %MODULE_NAME%ServiceCleanup(%MODULE_NAME%Service* service^) {
    echo     (void^)service;
    echo }
    echo.
    echo char* get%MODULE_NAME%Info(%MODULE_NAME%Service* service, Server* server^) {
    echo     (void^)service;
    echo     (void^)server;
    echo.
    echo     char* result = malloc(256^);
    echo     if (!result^) return NULL;
    echo.
    echo     createJsonObject(result, 256, 2,
    echo         "message", "Hello from %MODULE_NAME% module!",
    echo         "port", "8080"^);
    echo.
    echo     return result;
    echo }
) > "%TARGET_DIR%\%MODULE_NAME%.service.c"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to create %MODULE_NAME%.service.c
    exit /b 1
)

echo [INFO] Creating %MODULE_NAME%.module.h
(
    echo #ifndef %MODULE_NAME%_MODULE_H
    echo #define %MODULE_NAME%_MODULE_H
    echo.
    echo #include "../../shared/http/server/server.h"
    echo #include "../../shared/http/router/routes.h"
    echo.
    echo /// @brief Initializes the module
    echo /// @param server Pointer to the server
    echo void %MODULE_NAME%ModuleInit(Server* server^);
    echo.
    echo /// @brief Cleans up module resources
    echo /// @param server Pointer to the server
    echo void %MODULE_NAME%ModuleCleanup(Server* server^);
    echo.
    echo #endif // %MODULE_NAME%_MODULE_H
) > "%TARGET_DIR%\%MODULE_NAME%.module.h"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to create %MODULE_NAME%.module.h
    exit /b 1
)

echo [INFO] Creating %MODULE_NAME%.module.c
(
    echo #include "%MODULE_NAME%.module.h"
    echo #include "%MODULE_NAME%.controller.h"
    echo #include "%MODULE_NAME%.service.h"
    echo.
    echo #include "../../shared/http/server/server.h"
    echo #include "../../shared/http/router/routes.h"
    echo #include <stdlib.h>
    echo.
    echo static %MODULE_NAME%Service* service = NULL;
    echo.
    echo void %MODULE_NAME%ModuleInit(Server* server^) {
    echo     if (!server^) return;
    echo.
    echo     service = (%MODULE_NAME%Service*^)malloc(sizeof(%MODULE_NAME%Service^));
    echo     if (!service^) return;
    echo.
    echo     %MODULE_NAME%ServiceInit(service^);
    echo.
    echo     int count = 0;
    echo     const RouteConfig* routes = get%MODULE_NAME%Routes(^&count^);
    echo     if (routes ^&^& count ^> 0^) {
    echo         for (int i = 0; i ^< count; i++^) {
    echo             addRouteMethod(server, routes[i].path, routes[i].handler, routes[i].method^);
    echo         }
    echo     }
    echo.
    echo     printf("[\x1b[32mINFO\x1b[0m] %MODULE_NAME% module initialized\n");
    echo }
    echo.
    echo void %MODULE_NAME%ModuleCleanup(Server* server^) {
    echo     (void^)server;
    echo.
    echo     if (service^) {
    echo         %MODULE_NAME%ServiceCleanup(service^);
    echo         free(service^);
    echo         service = NULL;
    echo     }
    echo }
) > "%TARGET_DIR%\%MODULE_NAME%.module.c"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to create %MODULE_NAME%.module.c
    exit /b 1
)

echo.
echo [INFO] Module '%MODULE_NAME%' created successfully!
echo.
echo [INFO] Next steps:
echo [INFO] 1. Initialize the %MODULE_NAME%
echo --> Add ModuleInit(&server); function in main.c
echo [INFO] 2. Cleanup the module
echo --> Add ModuleCleanup(&server); function in main.c
echo [INFO] 3. Access the Request.http on root directory
echo --> Use GET http://localhost:8080/%MODULE_NAME%
echo --> Expected output: {\"message\": \"Hello from %MODULE_NAME% module!\", \"port\": \"8080\"}
echo.

endlocal
exit /b 0