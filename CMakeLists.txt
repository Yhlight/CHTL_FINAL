# CMakeList.txt for the CHTL project

# Require CMake 3.10 or higher
cmake_minimum_required(VERSION 3.10)

# Project definition
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add an executable
add_executable(chtl_compiler
    main.cpp
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp
)

# Add include directories to the target
target_include_directories(chtl_compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "CHTL project configured. Executable 'chtl_compiler' created.")
