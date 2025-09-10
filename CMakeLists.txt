cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add CHTL library
add_subdirectory(CHTL)

# Add scanner library
add_subdirectory(Scanner)

# Add the subdirectory for the main executable
add_subdirectory(CompilerDispatcher)

# --- Testing ---
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(Test)
