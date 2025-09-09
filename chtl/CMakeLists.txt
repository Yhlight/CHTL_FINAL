cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/lexer/lexer.cpp
    src/lexer/token.cpp
    src/parser/parser.cpp
    src/ast/ast_node.cpp
)

# Header files
set(HEADERS
    include/lexer/lexer.h
    include/lexer/token.h
    include/parser/parser.h
    include/ast/ast_node.h
)

# Create executable
add_executable(chtl ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(chtl)

# Create library
add_library(chtl_lib STATIC ${SOURCES} ${HEADERS})
target_link_libraries(chtl_lib)

# Install targets
install(TARGETS chtl chtl_lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers
install(DIRECTORY include/ DESTINATION include)

# Testing
enable_testing()
add_subdirectory(test)

# Examples
add_subdirectory(examples)