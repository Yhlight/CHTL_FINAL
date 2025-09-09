cmake_minimum_required(VERSION 3.10)

project(CHTL_FINAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src/Util)
add_subdirectory(src/CHTL)

add_executable(chtl src/main.cpp)

target_link_libraries(chtl PRIVATE Util CHTL)
