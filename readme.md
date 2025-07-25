# Webserver in C
A Minimal HTTP server in C. <br/>
- Outputs a JSON response on port 8080.
- Uses sockets (`sys/socket.h`, `netinet/in.h`). 
- Sends the JSON response.
- Accepts HTML client connections in a loop.<br/> 
  - Returns (ok) --> HTTP 200 status code.

## Index
- [Windows](#windows)
- [Ubuntu](#ubuntu)
- [macOS](#macos)

## Quick Start

### Windows
**Environment**:
1. Install Chocolatey: [choco install](https://chocolatey.org/install)
2. Install MinGW: `choco install mingw`
3. Add MinGW to PATH: `C:\ProgramData\chocolatey\lib\mingw\tools\install\mingw64\bin`

**Compile**:
```
gcc  webserver.c -o webserver -lws2_32
```

**Run**:
```
.\webserver.exe
```

### Ubuntu
**Environment**:
1. Install GCC: `sudo apt update && sudo apt install build-essential`

**Compile**:
```
gcc  webserver.c -o webserver
```

**Run**:
```
./webserver
```

### macOS
**Environment**:
1. Install Homebrew: [brew install](https://brew.sh)
2. Install GCC: `brew install gcc`

**Compile**:
```
gcc  webserver.c -o webserver
```

**Run**:
```
./webserver
```