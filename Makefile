# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I. -static

# Source files
SRCS = main.cpp \
       CHTL/CHTLLexer/Lexer.cpp \
       CHTL/CHTLLexer/Token.cpp \
       CHTL/CHTLParser/Parser.cpp \
       CHTL/CHTLGenerator/Generator.cpp \
       CHTL/CHTLNode/CustomStyleTemplateNode.cpp \
       CHTL/CHTLNode/StyleRuleNode.cpp \
       CHTL/CHTLNode/OriginNode.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = chtl_compiler

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
