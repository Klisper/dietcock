# ==========================================
# Makefile for dietcock (Clang / Clang++)
# ==========================================

# Compilers
CXX = clang++
CC = clang

# Compiler Flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O0
CFLAGS = -Wall -Wextra -O0

# Include directories (adjust if headers are in a separate folder)
INCLUDES = -I.

# Libraries and Frameworks
# --- Linux Configuration ---
LIBS = -lglfw -lGL -ldl -lpthread -lX11 -lXrandr -lXi

# --- macOS Configuration (Uncomment below if on macOS) ---
# LIBS = -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# Source Files
SRC_CPP = dietcock.cpp
SRC_C = shader.c glad.c

# Object Files mapping
OBJ_CPP = $(SRC_CPP:.cpp=.o)
OBJ_C = $(SRC_C:.c=.o)
OBJS = $(OBJ_CPP) $(OBJ_C)

# Output Executable Name
TARGET = dietcock

# Default Build Target
all: $(TARGET)

# Linking step
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# Compile C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile C source files (glad.c, shader.c)
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Declare non-file targets
.PHONY: all clean
