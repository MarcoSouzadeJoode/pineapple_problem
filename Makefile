# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lm

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build


# Source files
SRCFILES = $(wildcard $(SRCDIR)/*.c)
OBJFILES = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCFILES))

# Output binary
TARGET = $(BUILDDIR)/simulation

# Default target
all: $(TARGET)

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Build object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create the final binary
$(TARGET): $(OBJFILES)
	$(CC) $(OBJFILES) -o $@ $(LDFLAGS)

# Run the simulation
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR)/*

# Phony targets
.PHONY: all run clean

