cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Source files for simple test
set(SIMPLE_SOURCES
    CHTL/CHTLLexer/Token.cpp
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLNode/BaseNode.cpp
    CHTL/CHTLNode/RootNode.cpp
    CHTL/CHTLNode/ElementNode.cpp
    CHTL/CHTLNode/TextNode.cpp
)

# Create simple test executable
add_executable(test_simple test_simple.cpp ${SIMPLE_SOURCES})

# Create parser test executable
add_executable(test_parser test_parser.cpp ${SIMPLE_SOURCES})

# Create demo executable
add_executable(demo demo.cpp ${SIMPLE_SOURCES})

# Main executable
add_executable(chtl main.cpp ${SIMPLE_SOURCES})