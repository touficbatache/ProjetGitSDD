# Makefile pour lit.c et lit.h

CC = gcc
CFLAGS = -Wall -g

# Source file and header file
SRC = lit.c
HEADER = lit.h

# Output binary
TARGET = lit

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
