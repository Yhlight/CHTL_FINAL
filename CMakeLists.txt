cmake_minimum_required(VERSION 3.10)
project(CHTL_Project)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Util library
add_subdirectory(Util)

# Add CHTL library
add_subdirectory(CHTL)

# Add Tests
add_subdirectory(Test)
