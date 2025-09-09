cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Include directories
include_directories(include)

# Source directories
set(CHTL_SOURCES
    src/CHTL/CHTLContext/CHTLContext.cpp
    src/CHTL/CHTLGenerator/CHTLGenerator.cpp
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLLoader/CHTLLoader.cpp
    src/CHTL/CHTLManage/CHTLManage.cpp
    src/CHTL/CHTLNode/CHTLNode.cpp
    src/CHTL/CHTLParser/CHTLParser.cpp
    src/CHTL/CHTLState/CHTLState.cpp
    src/CHTL/CHTLIOStream/CHTLIOStream.cpp
    src/CHTL/CMODSystem/CMODSystem.cpp
)

set(CHTLJS_SOURCES
    src/CHTLJS/CHTLJSContext/CHTLJSContext.cpp
    src/CHTLJS/CHTLJSGenerator/CHTLJSGenerator.cpp
    src/CHTLJS/CHTLJSLexer/CHTLJSLexer.cpp
    src/CHTLJS/CHTLJSLoader/CHTLJSLoader.cpp
    src/CHTLJS/CHTLJSManage/CHTLJSManage.cpp
    src/CHTLJS/CHTLJSNode/CHTLJSNode.cpp
    src/CHTLJS/CHTLJSParser/CHTLJSParser.cpp
    src/CHTLJS/CHTLJSState/CHTLJSState.cpp
    src/CHTLJS/CHTLJSIOStream/CHTLJSIOStream.cpp
    src/CHTLJS/CJMODSystem/CJMODSystem.cpp
)

set(UTIL_SOURCES
    src/Util/FileSystem/FileSystem.cpp
    src/Util/ZipUtil/ZipUtil.cpp
    src/Util/StringUtil/StringUtil.cpp
)

set(SCANNER_SOURCES
    src/Scanner/Scanner.cpp
)

set(CODE_MERGER_SOURCES
    src/CodeMerger/CodeMerger.cpp
)

set(COMPILER_DISPATCHER_SOURCES
    src/CompilerDispatcher/CompilerDispatcher.cpp
)

# Create the main CHTL library
add_library(chtl_core STATIC
    ${CHTL_SOURCES}
    ${CHTLJS_SOURCES}
    ${UTIL_SOURCES}
    ${SCANNER_SOURCES}
    ${CODE_MERGER_SOURCES}
    ${COMPILER_DISPATCHER_SOURCES}
)

# Create the main executable
add_executable(chtl src/main.cpp)
target_link_libraries(chtl chtl_core)

# Test executable
add_executable(chtl_test src/test_main.cpp)
target_link_libraries(chtl_test chtl_core)

# Enable testing
enable_testing()
add_test(NAME chtl_tests COMMAND chtl_test)