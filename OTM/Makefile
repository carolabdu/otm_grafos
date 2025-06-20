# Makefile for building test_exec

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -O3 -march=native -Ofast -flto
LDFLAGS_GUROBI = -fopenmp
GUROBI_HOME = ~/path/to/gurobi
GUROBI_INCLUDE = $(GUROBI_HOME)/include
GUROBI_LIB = $(GUROBI_HOME)/lib
GRB_LICENSE_FILE = /path/to/gurobi.lic

# Source files
SRCS = classes_base.cpp utils.cpp ils.cpp grasp.cpp react_grasp.cpp simulated_annealing.cpp tabu.cpp file_io.cpp test.cpp

# Output executables
TARGET = test_exec
GUROBI_EXEC = gurobi_metrics

# Default target
all: $(TARGET)

# Linking the main target
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# Gurobi target
gurobi:
	@echo "Compiling with Gurobi..."
	@export GUROBI_HOME=$(GUROBI_HOME); \
	export PATH=$$GUROBI_HOME/bin:$$PATH; \
	export LD_LIBRARY_PATH=$$GUROBI_HOME/lib:$$LD_LIBRARY_PATH; \
	export GRB_LICENSE_FILE=$(GRB_LICENSE_FILE); \
	$(CXX) $(CXXFLAGS) $(LDFLAGS_GUROBI) gurobi_parallel.cpp -I$$GUROBI_HOME/include -L$$GUROBI_HOME/lib -lgurobi_c++ -lgurobi120 -o $(GUROBI_EXEC)

# Clean target
clean:
	rm -f $(TARGET) $(GUROBI_EXEC)
