# CMakeLists.txt

# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(CHTL_FINAL CXX)

# Set the C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the subdirectory for the CHTL library
add_subdirectory(CHTL)

# Add the subdirectory for the tests
add_subdirectory(Test/ParserTest)
