# Usage:
# make FILE=your_program.c
# or make FILE=your_program  (if you omit .c)
# The output executable will have the same base name as the source file.

# Force mpicc to use gcc-15
export OMPI_CC := gcc-15
export MPICH_CC := gcc-15
CC = gcc-15

# Compiler wrapper
MPICC = mpicc

# Compilation flags
CFLAGS = -O1 -Wall -Wextra -march=native

# Pass the code file name (with or without .c extension)
FILE ?=

# Extract basename without extension
BASENAME := $(basename $(FILE))

# Default target
all: $(BASENAME)

# Compile target
$(BASENAME):
	$(MPICC) $(CFLAGS) $(FILE) -o $(BASENAME)

# Clean target
clean:
	rm -f $(BASENAME)
