cmake_minimum_required(VERSION 3.16)
project(CHTL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(scanner STATIC
    src/Scanner/UnifiedScanner.cpp
)
target_include_directories(scanner PUBLIC src)

add_library(dispatcher INTERFACE)
target_include_directories(dispatcher INTERFACE src)

add_library(codemerger INTERFACE)
target_include_directories(codemerger INTERFACE src)

add_library(chtl_core INTERFACE)
target_include_directories(chtl_core INTERFACE src)

add_library(chtljs_core INTERFACE)
target_include_directories(chtljs_core INTERFACE src)

add_executable(chtl_cli
    src/main.cpp
)
target_link_libraries(chtl_cli PRIVATE scanner)
