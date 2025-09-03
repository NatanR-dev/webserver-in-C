# Cross-platform Makefile for the webserver project

# Detect OS and set variables accordingly
ifeq ($(OS),Windows_NT)
    # Windows
    CC = gcc
    CFLAGS = -Wall -Wextra -I./src
    LDFLAGS = -lws2_32
    EXECUTABLE = webserver.exe
    RM = del /s /q
    MKDIR = mkdir
    RMDIR = rmdir /s /q
    SEP = \\
    FIXPATH = $(subst /,\,$1)
    RUN_CMD = .\$(EXECUTABLE)
else
    # Unix-like
    CC ?= gcc
    CFLAGS = -Wall -Wextra -I./src
    LDFLAGS =
    EXECUTABLE = webserver
    RM = rm -f
    MKDIR = mkdir -p
    RMDIR = rm -rf
    SEP = /
    FIXPATH = $1
    RUN_CMD = ./$(EXECUTABLE)
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

# Main target
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Build successful: $(EXECUTABLE)"

# Pattern rule for .c files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_SUBDIRS)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Create obj directories
$(OBJ_SUBDIRS):
	@$(MKDIR) $(call FIXPATH,$@)

# Clean
clean:
	@echo "Cleaning..."
	@$(RMDIR) $(call FIXPATH,$(OBJ_DIR)) 2>nul || exit 0
	@$(RM) $(call FIXPATH,$(EXECUTABLE)) 2>nul || exit 0
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
