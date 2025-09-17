# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I. -g

# Executable name
EXEC = chtl_compiler

# Source files
# Use find to automatically get all .cpp files
SRCS = $(shell find . -name "*.cpp")

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
	./$(EXEC) Test/custom_features.chtl

.PHONY: all clean run
