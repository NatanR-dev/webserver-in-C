# Webserver in C
A Minimal cross-platform HTTP server built in C. <br/>

https://github.com/user-attachments/assets/97f9860b-77f3-4874-b2e3-84e0eabbd061

- Opens a 8080 port.
  - Provides a Webserver API with routing system.
  - Root path with hateoas listing:
    - GET  /api/machine
    - POST /api/machine
    - GET  /api/os
    - GET  /api/system
- Uses sockets (`sys/socket.h`, `netinet/in.h`). 
  - Threads.
  - JSON formatting.
  - Platform-specific implementations.
- Supports Client connections in a loop (handle requests).<br/> 
  - MAX ROUTES 
  - BUFFER_SIZE 
  - MAX_CLIENTS 
  - PORT 
  - Logs 
- Responds with a JSON or HTML. <br/>
  - GET returns (ok) 
    - HTTP 200 status code.
  - POST returns (created) 
    - HTTP 201 status code.
  - GET returns (not found) 
    - HTTP 404 status code.
  - GET returns (bad request) 
    - HTTP 400 status code.
  - GET/POST returns (internal server error) 
    - HTTP 500 status code.
<br/>  

---  

### The Challenge: 
A <b>limited</b> Procedure language adapted to modular-structure <br/> 
like modern frameworks: <i>Nest, Express.</i>

- <b>Idea:</b> Use include headers to import functions as componentized-style and build a makefile to construct the procedure line logic to compile the project.



### Project Arch
```
webserver-in-C/
├── src/
│   ├── root/                    # Root module 
│   │   ├── root.controller.*    # Route routes / handlers
│   │   ├── root.module.*        # Module initialization
│   │   └── root.service.*       # Service logic
│   │
│   ├── system/                  # System specs module 
│   │   ├── system.controller.*  # System routes / handlers
│   │   ├── system.module.*      # Module initialization
│   │   └── system.service.*     # System info logic
│   │
│   ├── shared/                  # Shared utilities and components
│   │   ├── formats/             # Data formatting (JSON, etc.)
│   │   ├── http/                # HTTP protocol handling
│   │   │   ├── network/         # Network utilities
│   │   │   ├── response/        # HTTP response handling
│   │   │   ├── router/          # Routing system and decorators
│   │   │   └── server/          # Core HTTP server implementation
│   │   └── platform/            # Platform-specific implementations
│   │
│   └── main.c                   # Initialize the webserver
│
├── client/                      # Simple Frontend to consume the API
│   ├── css/
│   ├── js/
│   └── index.html
│
├── obj/                         # Compiled objects
└── Makefile                     # Build configuration

Routes:
-> GET / - Welcome (Root Path with Hateoas)
-> GET /api - Example API response
-> GET /api/machine - Machine status
-> POST /api/machine - Add machine name
-> GET /api/os - OS info
-> GET /api/system - System Specs
```

---

## Plataforms
- [Windows](#windows)
- [Linux](#linux-debian-based)
- [macOS](#macos)

## Quick Start

### Windows
**Environment**:
1. Install MSYS2 (recommended):
   - Download from: [MSYS2 Installer](https://github.com/msys2/msys2-installer)
   - Follow installation instructions
   - In MSYS2 terminal, run:
     ```bash
     pacman -Syu
     pacman -S --needed base-devel mingw-w64-x86_64-toolchain
     ```
   - Add to PATH: `C:\msys64\mingw64\bin`

**Build and Run**:
```bash
# Build the project
make

# Build and run the server
make run

# Clean build artifacts
make clean
```

**Tips: Run in Background (PowerShell)**:
```powershell
# Start server in background
Start-Job -ScriptBlock { .\webserver.exe }

# Check if the job is running
Get-Job

# Stop server
Stop-Job -Id <JOB_ID>
Remove-Job -Id <JOB_ID>
```

### Linux (Debian-based)
**Environment**:
1. Install build tools:
   ```bash
   sudo apt update
   sudo apt install build-essential
   ```

**Build and Run**:
```bash
# Build the project
make

# Build and run the server
make run

# Clean build artifacts
make clean
```

**Tips: Run in Background**:
```bash
# Start server in background
./webserver &

# Check running processes
ps aux | grep webserver

# To stop the server
kill $(pgrep webserver)
```

### macOS
**Environment**:
1. Install Homebrew: [brew install](https://brew.sh)
2. Install build tools:
   ```bash
   brew install gcc make
   ```

**Build and Run**:
```bash
# Build the project
make

# Build and run the server
make run

# Clean build artifacts
make clean
```

---

>_Hey, if you find this project useful, please consider giving it a ⭐ on [GitHub](https://github.com/yourusername/webserver-in-C)!_

## Contributing
Contributions are welcome! Please see [CONTRIBUTING](CONTRIBUTING.md) for guidelines.

>Feel free to fork the repository and submit pull requests with your improvements.

## License
[![GNU GPLv3](https://www.gnu.org/graphics/gplv3-127x51.png)](https://www.gnu.org/licenses/gpl-3.0)

This project is licensed under the [GNU GPLv3](LICENSE) © 2025

<div style="text-align:center;">
<br/>
<br/>
<h2 alt="Hail Stallman!">🤘Hɐıan Sʇanɯau</h2>
<br/>
<a href="https://www.gnu.org/philosophy/free-sw.html"><img src="https://upload.wikimedia.org/wikipedia/commons/8/83/The_GNU_logo.png" width="250px" _target="_blank"></a>
</div>
