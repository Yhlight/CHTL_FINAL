# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Set the project name and version
project(CHTL VERSION 1.0)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the main CHTL source directory
add_subdirectory(CHTL)

# Add the test directory
add_subdirectory(Test)
