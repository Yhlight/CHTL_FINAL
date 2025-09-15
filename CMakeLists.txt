cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Define the executable
add_executable(chtl_compiler src/main.cpp)

# Define the source files for the CHTL library
set(CHTL_LIB_SOURCES
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLParser/CHTLParser.cpp
    src/CHTL/CHTLGenerator/CHTLGenerator.cpp
    src/CHTL/CHTLLoader/CHTLLoader.cpp
    src/Scanner/CHTLUnifiedScanner.cpp
    src/CompilerDispatcher/CompilerDispatcher.cpp
    src/CHTLJS/CHTLJSLexer/CHTLJSLexer.cpp
    src/CHTLJS/CHTLJSParser/CHTLJSParser.cpp
    src/CHTLJS/CHTLJSGenerator/CHTLJSGenerator.cpp
    src/CHTLJS/CHTLJSplitter.cpp
    src/CodeMerger/CodeMerger.cpp
)

# Create a static library for our compiler components
add_library(chtl_lib STATIC ${CHTL_LIB_SOURCES})

# Link the executable against our library
target_link_libraries(chtl_compiler PRIVATE chtl_lib)

# Add the src directory to the include path for the executable and the library
target_include_directories(chtl_compiler PUBLIC src)
target_include_directories(chtl_lib PUBLIC src)

# Configuration for the new test runner
add_executable(test_runner tests/test_runner.cpp)
target_link_libraries(test_runner PRIVATE stdc++fs)

# CHTLJS Parser Test runner
add_executable(test_parser tests/test_parser.cpp)
target_link_libraries(test_parser PRIVATE chtl_lib)

# CHTL Unified Scanner Test runner
add_executable(test_scanner tests/test_scanner.cpp)
target_link_libraries(test_scanner PRIVATE chtl_lib)

# CHTL JS Compiler Test runner
add_executable(test_chtljs_compiler tests/test_chtljs_compiler.cpp)
target_link_libraries(test_chtljs_compiler PRIVATE chtl_lib)

# Add the new single test command
add_test(NAME SnapshotTests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND test_runner)
