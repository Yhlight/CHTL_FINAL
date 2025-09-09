cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/CHTLCompiler)
include_directories(${CMAKE_SOURCE_DIR}/CHTLJS)
include_directories(${CMAKE_SOURCE_DIR}/Scanner)
include_directories(${CMAKE_SOURCE_DIR}/CompilerDispatcher)
include_directories(${CMAKE_SOURCE_DIR}/CodeMerger)
include_directories(${CMAKE_SOURCE_DIR}/CSS)
include_directories(${CMAKE_SOURCE_DIR}/JS)
include_directories(${CMAKE_SOURCE_DIR}/Util)
include_directories(${CMAKE_SOURCE_DIR}/Test)

# Source files
set(CHTL_SOURCES
    main.cpp
    CHTLCompiler/CHTLContext/CHTLContext.cpp
    CHTLCompiler/CHTLLexer/CHTLLexer.cpp
    CHTLCompiler/CHTLNode/CHTLNode.cpp
    CHTLCompiler/CHTLParser/CHTLParser.cpp
    # Add other source files as they are created
)

# Header files
set(CHTL_HEADERS
    CHTLCompiler/CHTLContext/CHTLContext.h
    CHTLCompiler/CHTLLexer/CHTLLexer.h
    CHTLCompiler/CHTLNode/CHTLNode.h
    CHTLCompiler/CHTLParser/CHTLParser.h
    # Add other header files as they are created
)

# Create executable
add_executable(chtl ${CHTL_SOURCES} ${CHTL_HEADERS})

# Link libraries
target_link_libraries(chtl)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(chtl PRIVATE -Wall -Wextra -pedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(chtl PRIVATE -Wall -Wextra -pedantic)
endif()

# Debug flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(chtl PRIVATE DEBUG_MODE=1)
    target_compile_options(chtl PRIVATE -g -O0)
else()
    target_compile_options(chtl PRIVATE -O2)
endif()