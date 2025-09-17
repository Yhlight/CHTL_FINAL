cmake_minimum_required(VERSION 3.10)
project(CHTL_FINAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Gather all source files for the compiler library
file(GLOB_RECURSE CHTL_SOURCES
    "CHTL/CHTLLexer/*.cpp"
    "CHTL/CHTLNode/*.cpp"
    "CHTL/CHTLParser/*.cpp"
    "Util/StringUtil/*.cpp"
    "Util/FileSystem/*.cpp"
)

# Define the compiler library
add_library(chtl_lib ${CHTL_SOURCES})

# Specify include directories for the library
target_include_directories(chtl_lib PUBLIC
    "${PROJECT_SOURCE_DIR}"
)

# Define the main executable that will run the compiler
add_executable(chthl_runner main.cpp)

# Link the executable against the compiler library
target_link_libraries(chthl_runner PRIVATE chtl_lib)
