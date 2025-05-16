# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

# Directories
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Files
TARGET = $(BIN_DIR)/server
SOURCES = $(SRC_DIR)/sensor_reader.c $(SRC_DIR)/http_server.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Default rule
all: $(TARGET)

# Rule to create the target executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $(TARGET) -lmicrohttpd

# Rule to compile the .c files to .o object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean
