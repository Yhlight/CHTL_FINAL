cmake_minimum_required(VERSION 3.10)

project(CHTL_FINAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# Add the 'src' directory as a top-level include directory
include_directories(src)

add_subdirectory(src/Util)
add_subdirectory(src/CHTL)
add_subdirectory(src/CHTLLoader)

add_executable(chtl src/main.cpp)

target_link_libraries(chtl PRIVATE Util CHTL CHTLLoader)
