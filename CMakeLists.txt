cmake_minimum_required(VERSION 3.10)
project(CHTLCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add all source files
file(GLOB_RECURSE SOURCES
    "CHTL/CHTLContext/*.cpp"
    "CHTL/CHTLGenerator/*.cpp"
    "CHTL/CHTLLexer/*.cpp"
    "CHTL/CHTLParser/*.cpp"
    "CHTL/CHTLNode/*.cpp"
    "CHTL/CHTLIOStream/*.cpp"
    "Util/StringUtil/*.cpp"
    "Util/FileSystem/*.cpp"
    "Util/ZipUtil/*.cpp"
)

add_executable(chtl_compiler main.cpp ${SOURCES})

# Include directories
target_include_directories(chtl_compiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
