cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add all sources directly to the executable
add_executable(chtl_compiler
    src/main.cpp
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
)

# Target include directories
target_include_directories(chtl_compiler PUBLIC src src/CJMOD)

# Link against filesystem
target_link_libraries(chtl_compiler PRIVATE stdc++fs)

# Add platform-specific libraries for dynamic loading
if (UNIX AND NOT APPLE)
    target_link_libraries(chtl_compiler PRIVATE dl)
endif()
