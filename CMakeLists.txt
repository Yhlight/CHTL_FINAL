# CHTL Project CMakeLists.txt
#
# NOTE: This file is currently NOT USED due to an issue with the execution
# environment's handling of the working directory, which prevents cmake from
# running correctly.
#
# The project is instead compiled directly with g++. The correct command is:
# g++ -std=c++17 -I/app/ -I/app/CHTL/CHTLLexer/ -I/app/CHTL/CHTLNode/ -I/app/CHTL/CHTLParser/ -I/app/CHTL/CHTLGenerator/ /app/main.cpp /app/CHTL/CHTLLexer/Lexer.cpp /app/CHTL/CHTLLexer/Token.cpp /app/CHTL/CHTLParser/Parser.cpp /app/CHTL/CHTLGenerator/Generator.cpp -o /app/chtl


# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(CHTL CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(
    CHTL/CHTLLexer
    CHTL/CHTLNode
    CHTL/CHTLParser
    CHTL/CHTLGenerator
)

# Add an executable for the compiler.
add_executable(chtl main.cpp
    CHTL/CHTLLexer/Token.cpp
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp
)
