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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Include directories
include_directories(include)
include_directories(src)
include_directories(src/CHTL)
include_directories(src/CHTLJS)

# Source files
file(GLOB_RECURSE CHTL_SOURCES 
    "src/CHTL/*.cpp"
    "src/CHTL/*.h"
)

file(GLOB_RECURSE CHTLJS_SOURCES 
    "src/CHTLJS/*.cpp"
    "src/CHTLJS/*.h"
)

file(GLOB_RECURSE CSS_SOURCES 
    "src/CSS/*.cpp"
    "src/CSS/*.h"
)

file(GLOB_RECURSE JS_SOURCES 
    "src/JS/*.cpp"
    "src/JS/*.h"
)

file(GLOB_RECURSE SCANNER_SOURCES 
    "src/Scanner/*.cpp"
    "src/Scanner/*.h"
)

file(GLOB_RECURSE UTIL_SOURCES 
    "src/Util/*.cpp"
    "src/Util/*.h"
)

# Main executable
add_executable(chtl 
    src/main.cpp
    ${CHTL_SOURCES}
    ${CHTLJS_SOURCES}
    ${CSS_SOURCES}
    ${JS_SOURCES}
    ${SCANNER_SOURCES}
    ${UTIL_SOURCES}
)

# Test executable (optional)
# add_executable(chtl_test
#     src/Test/main_test.cpp
#     ${CHTL_SOURCES}
#     ${CHTLJS_SOURCES}
#     ${CSS_SOURCES}
#     ${JS_SOURCES}
#     ${SCANNER_SOURCES}
#     ${UTIL_SOURCES}
# )

# Enable testing (when test executable is available)
# enable_testing()
# add_test(NAME chtl_tests COMMAND chtl_test)

# Installation
install(TARGETS chtl DESTINATION bin)
install(DIRECTORY modules/ DESTINATION share/chtl/modules)