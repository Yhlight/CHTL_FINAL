cmake_minimum_required(VERSION 3.10)

# Project Name
project(CHTL_Compiler CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories for the entire project
include_directories(
    "src/CHTL/CHTLGenerator"
    "src/CHTL/CHTLLexer"
    "src/CHTL/CHTLNode"
    "src/CHTL/CHTLParser"
    "src/Util/StringUtil"
    "src/Util/FileSystem"
    "src/Compiler"
)

# Create a static library for the compiler core
add_library(chthl_lib STATIC
    src/CHTL/CHTLLexer/Token.cpp
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLParser/CHTLParser.cpp
    src/CHTL/CHTLGenerator/CHTLGenerator.cpp
    src/Compiler/Compiler.cpp
)

# Add the main executable
add_executable(chtl src/Compiler/main.cpp)

# Link the executable against the compiler library
target_link_libraries(chtl chthl_lib)

# --- Testing ---
enable_testing()

# Add the test executable
add_executable(compiler_test tests/compiler_test.cpp)

# Link the test executable against the compiler library
target_link_libraries(compiler_test chthl_lib)

# Add the test to CTest
add_test(NAME ChtlCompilerTest COMMAND compiler_test)

set_tests_properties(ChtlCompilerTest PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

message(STATUS "CHTL Compiler project configured for testing")
