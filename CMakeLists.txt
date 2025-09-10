cmake_minimum_required(VERSION 3.10)

project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories for header files
include_directories(
    Util
    Scanner
    CHTL
    CHTL/CHTLNode
)

# List of source files
set(COMPILER_SOURCES
    src/main.cpp
    Util/StringUtil.cpp
    Util/FileSystem.cpp
    Scanner/UnifiedScanner.cpp
    CHTL/CHTLLexer.cpp
    CHTL/CHTLParser.cpp
    CHTL/CHTLGenerator.cpp
)

add_executable(chtl-compiler ${COMPILER_SOURCES})
