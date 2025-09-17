# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I. -g

# Executable name
EXEC = chtl_compiler

# Find all .cpp files recursively
SRCS = $(shell find CHTL -name "*.cpp") main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(EXEC)

# Linking the executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS)

# Compiling source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(EXEC) $(OBJS)

# A simple run target for convenience
run: all
	./$(EXEC) Test/basic.chtl

.PHONY: all clean run
