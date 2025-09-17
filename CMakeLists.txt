cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create a static library with all the core components
add_library(chtl_lib STATIC
    miniz.c
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLParser/CHTLParser.cpp
    src/CHTL/CHTLGenerator/CHTLGenerator.cpp
    src/CHTL/CHTLLoader/CHTLLoader.cpp
    src/Scanner/CHTLUnifiedScanner.cpp
    src/CHTLJS/CHTLJSLexer/CHTLJSLexer.cpp
    src/CHTLJS/CHTLJSParser/CHTLJSParser.cpp
    src/CHTLJS/CHTLJSGenerator/CHTLJSGenerator.cpp
    src/CJMOD/API/Syntax.cpp
    src/CJMOD/Scanner/CJMODScanner.cpp
    src/CodeMerger/CodeMerger.cpp
)

# Public include directories for the library
target_include_directories(chtl_lib PUBLIC src src/CJMOD ${CMAKE_CURRENT_SOURCE_DIR}/third_party/miniz)
target_compile_features(chtl_lib PUBLIC cxx_std_17)

# Main compiler executable
add_executable(chtl_compiler src/main.cpp)
target_link_libraries(chtl_compiler PRIVATE chtl_lib)
if (UNIX AND NOT APPLE)
    target_link_libraries(chtl_compiler PRIVATE dl)
endif()

# Test executables
add_executable(test_parser tests/test_parser.cpp)
target_link_libraries(test_parser PRIVATE chtl_lib)

# add_executable(test_scanner tests/test_scanner.cpp)
# target_link_libraries(test_scanner PRIVATE chtl_lib)

# add_executable(test_unified_scanner tests/test_unified_scanner.cpp)
# target_link_libraries(test_unified_scanner PRIVATE chtl_lib)

add_executable(test_runner tests/test_runner.cpp)
target_link_libraries(test_runner PRIVATE chtl_lib)

# add_executable(test_scanner_v2 tests/test_scanner_v2.cpp)
# target_link_libraries(test_scanner_v2 PRIVATE chtl_lib)
