# Makefile for building test_exec

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -O3 -march=native -Ofast -flto

# Source files
SRCS = classes_base.cpp utils.cpp ils.cpp grasp.cpp simulated_annealing.cpp tabu.cpp file_io.cpp test.cpp

# Output executable
TARGET = test_exec

# Default target
all: $(TARGET)

# Linking the target
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# Clean target
clean:
	rm -f $(TARGET)
