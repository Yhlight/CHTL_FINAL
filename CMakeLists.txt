cmake_minimum_required(VERSION 3.10)
project(CHTL_FINAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gather all cpp files for the executable
set(SOURCES
    src/main.cpp
    src/CHTL/CHTLLexer/Lexer.cpp
    src/CHTL/CHTLParser/Parser.cpp
    src/CHTL/CHTLGenerator/Generator.cpp
)

add_executable(chtl ${SOURCES})

include_directories(src)
