cmake_minimum_required(VERSION 3.10)
project(printMylove_cjmod CXX)

# Add the source file and compile it as a SHARED library
add_library(printMylove_cjmod SHARED printMylove.cpp)

# Set the C++ standard
set_target_properties(printMylove_cjmod PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# We need to find the main chttl_lib to link against so we can get the CJMOD API headers
# This assumes the main project is already configured.
target_include_directories(printMylove_cjmod PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Place the output in the root "module" directory so the loader can find it.
set_target_properties(printMylove_cjmod PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/module"
)
