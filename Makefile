# Cross-platform Makefile for the webserver project

# Set the shell to cmd.exe on Windows to ensure compatibility
ifeq ($(OS),Windows_NT)
    SHELL = cmd.exe
    .SHELLFLAGS = /c
    # Create necessary directories if they don't exist
    $(shell if not exist "backend\obj" mkdir backend\obj)
    $(shell if not exist "backend\obj\shared" mkdir backend\obj\shared)
    $(shell if not exist "backend\obj\shared\validation" mkdir backend\obj\shared\validation)
endif

# Default build type (can be overridden with: make BUILD=debug)
BUILD ?= release

# Detect OS and set variables accordingly
ifeq ($(OS),Windows_NT)
    # Windows configuration
    CC = gcc
    CFLAGS = -Wall -Wextra -D_WIN32_WINNT=0x0601 -I$(SRC_DIR) -I$(SRC_DIR)/shared -I$(SRC_DIR)/shared/http -I$(SRC_DIR)/root -I$(SRC_DIR)/system -I$(SRC_DIR)/shared/formats/json -I$(SRC_DIR)/shared/platform -I$(SRC_DIR)/shared/validation
    LDFLAGS = -lws2_32 -lwinmm -liphlpapi -lrpcrt4 -lole32 -loleaut32 -luuid -lwbemuuid -ladvapi32 -lshell32
    EXECUTABLE = webserver.exe
    RM = del /f /q
    RMDIR = rmdir /s /q
    MKDIR = mkdir
    SEP = \\
    FIXPATH = $(subst /,\,$1)
    RUN_CMD = .\$(EXECUTABLE)
    
    # Windows-specific commands
    RM_CMD = $(RM) $(1) $(2) >nul 2>&1 || exit 0
    RMDIR_CMD = if exist $(1) $(RMDIR) $(1) >nul 2>&1
    
    # Add Windows-specific defines
    CFLAGS += -DPLATFORM_WINDOWS -D_WIN32 -DWIN32 -D_WINDOWS
else
    # Unix-like configuration (Linux, macOS, etc.)
    CC ?= gcc
    CFLAGS = -Wall -Wextra -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/shared -I$(INCLUDE_DIR)/shared/http -I$(SRC_DIR)/root -I$(SRC_DIR)/system -I$(INCLUDE_DIR)/shared/formats/json -I$(INCLUDE_DIR)/shared/platform -I$(INCLUDE_DIR)/shared/validation
    LDFLAGS = -pthread  # For multi-threading support on Unix-like systems
    EXECUTABLE = webserver
    RM = rm -f
    MKDIR = mkdir -p
    RMDIR = rm -rf
    SEP = /
    FIXPATH = $1
    RUN_CMD = ./$(EXECUTABLE)
    
    # Unix-like commands
    RM_CMD = $(RM) $(1) 2>/dev/null || true
    RMDIR_CMD = $(RMDIR) $(1) 2>/dev/null || true
endif

# Set build-specific flags
ifeq ($(BUILD),debug)
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

# Source files
SRC_DIR = backend/src
INCLUDE_DIR = $(SRC_DIR)

# Source files
MAIN_SRC = $(SRC_DIR)/main.c

# Server source files
SERVER_SRCS = $(SRC_DIR)/shared/http/server/server.c

# Platform-specific source files
ifeq ($(OS),Windows_NT)
    PLATFORM_SRCS = $(SRC_DIR)/shared/platform/windows/platform.c
else
    PLATFORM_SRCS = $(SRC_DIR)/shared/platform/unix/platform.c
endif

# Shared source files
SHARED_SRCS = $(SRC_DIR)/shared/http/server/server.c \
              $(SRC_DIR)/shared/http/server/client.handler.c \
              $(SRC_DIR)/shared/http/response/response.c \
              $(SRC_DIR)/shared/http/router/router.c \
              $(SRC_DIR)/shared/http/router/routes.c \
              $(SRC_DIR)/shared/formats/json/json.c \
              $(PLATFORM_SRCS) \
              $(wildcard $(SRC_DIR)/shared/validation/*.c)

# Module source files
MODULE_SRCS = $(SRC_DIR)/root/root.module.c \
              $(SRC_DIR)/root/root.controller.c \
              $(SRC_DIR)/root/root.service.c \
              $(SRC_DIR)/system/system.module.c \
              $(SRC_DIR)/system/system.controller.c \
              $(SRC_DIR)/system/system.service.c

# All source files
SRC = $(MAIN_SRC) \
      $(SHARED_SRCS) \
      $(MODULE_SRCS)

# Object files
OBJ_DIR = obj
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

# Create obj directory structure
OBJ_SUBDIRS = $(sort $(dir $(OBJ)))

# Main targets
.PHONY: all debug release clean run

# Default build (release)
all: release

# Debug build with debug symbols
debug: BUILD = debug
debug: $(EXECUTABLE)

# Release build with optimizations
release: BUILD = release
release: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	@echo "Linking $@ ($(BUILD) build)..."
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Build successful: $(EXECUTABLE) ($(BUILD))"

# Run the application

# Pattern rule for .c files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_SUBDIRS)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Create obj directories
$(OBJ_SUBDIRS):
	@$(MKDIR) $(call FIXPATH,$@)

# Create object directory structure
$(OBJ_DIR):
	@$(MKDIR) $(call FIXPATH,$(OBJ_DIR))
	@$(MKDIR) $(call FIXPATH,$(OBJ_DIR)/shared/http/server)
	@$(MKDIR) $(call FIXPATH,$(OBJ_DIR)/shared/http/response)
	@$(MKDIR) $(call FIXPATH,$(OBJ_DIR)/shared/http/router)
	@$(MKDIR) $(call FIXPATH,$(OBJ_DIR)/shared/platform/windows)
	@$(MKDIR) $(call FIXPATH,$(OBJ_DIR)/shared/platform/unix)
	@$(MKDIR) $(call FIXPATH,$(OBJ_DIR)/shared/formats/json)
	@$(MKDIR) $(call FIXPATH,$(OBJ_DIR)/root)
	@$(MKDIR) $(call FIXPATH,$(OBJ_DIR)/system)
	@$(MKDIR) $(call FIXPATH,$(OBJ_DIR)/shared/validation)

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@if exist "$(call FIXPATH,$(OBJ_DIR))" rmdir /s /q "$(call FIXPATH,$(OBJ_DIR))"
	@if exist "$(call FIXPATH,$(EXECUTABLE))" del /f /q "$(call FIXPATH,$(EXECUTABLE))"
	@if exist "$(call FIXPATH,$(EXECUTABLE).exe)" del /f /q "$(call FIXPATH,$(EXECUTABLE).exe)"
	@echo "Clean complete"

# Run the server
run: $(EXECUTABLE)
	@echo "Starting $(EXECUTABLE)..."
	@$(RUN_CMD)

# Help
help:
	@echo "\nAvailable targets:"
	@echo "  all     - Build the webserver (default)"
	@echo "  clean   - Remove object files and the executable"
	@echo "  run     - Build and run the webserver"
	@echo "  help    - Show this help message"

# List of phony targets
.PHONY: all clean run help
