# Cross-platform Makefile for the webserver project

# Set the shell to cmd.exe on Windows to ensure compatibility
ifeq ($(OS),Windows_NT)
    SHELL = cmd.exe
    .SHELLFLAGS = /c
endif

# Default build type (can be overridden with: make BUILD=debug)
BUILD ?= release

# Detect OS and set variables accordingly
ifeq ($(OS),Windows_NT)
    # Windows configuration
    CC = gcc
    CFLAGS = -Wall -Wextra -I./src
    LDFLAGS = -lws2_32 -liphlpapi
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
else
    # Unix-like configuration (Linux, macOS, etc.)
    CC ?= gcc
    CFLAGS = -Wall -Wextra -I./src
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
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.c) \
      $(wildcard $(SRC_DIR)/*/*.c)

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

# Create necessary directories
$(OBJ_DIR):
	@$(MKDIR) $(call FIXPATH,$(OBJ_DIR))

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
