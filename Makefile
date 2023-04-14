# Makefile pour main.c et main.h


CC = gcc
CFLAGS = -Wall -g

# Source file and header file
SRC = main.c
HEADER = main.h

# Output binary
TARGET = main

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

# Compilation
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $<

# Clean
clean:
	rm -f $(TARGET) *.o
