# Webserver in C
A Minimal cross-platform HTTP server in C. <br/>
- Opens a 8080 port.
  - Provides a Webserver API with routes.
- Uses sockets (`sys/socket.h`, `netinet/in.h`). 
- Responds with a JSON or HTML.
- Supports Client connections in a loop (handle requests).<br/> 
  - Returns (ok) --> HTTP 200 status code.
<br/>  

---  

### The Challenge: 
A <b>limited</b> Procedure language adapted to modular-structure <br/> 
like modern frameworks: <i>Nest, Express.</i>

- <b>Idea:</b> Use include headers to import functions as componentized-style and build a makefile to construct the procedure line logic to compile the project.



### Project Structure
```
webserver-in-C/
├── src/
│   ├── handlers/    # Request handlers
│   ├── routes/      # Route definitions
│   ├── server/      # Server logic
│   └── utils/       # Utility functions
├── obj/            # Compiled objects
├── Makefile        # Build configuration
└── src/main.c      # Entry point

Routes:
- GET / - Welcome message
- GET /api - Basic API response
- GET /os - OS information
- GET /system - System information
```

---

## Setup
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

