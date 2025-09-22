cmake_minimum_required(VERSION 3.10)
project(CHTL_Project C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- Define the CHTL Library ---
# Gather all source files for the library (excluding test and CLI main files)
file(GLOB_RECURSE CHTL_SOURCES
    "CHTL/*.cpp"
    "CHTL/*/*.cpp"
    "CHTL/*/*/*.cpp"
    "CHTL JS/*.cpp"
    "CHTL JS/*/*.cpp"
    "CHTL JS/*/*/*.cpp"
    "Scanner/*.cpp"
    "Util/*.cpp"
    "Util/*/*.cpp"
)

# Add miniz.c explicitly since it's a C file in the root directory
list(APPEND CHTL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/miniz.c")

# Create a static library from the source files
add_library(chtl_lib STATIC ${CHTL_SOURCES})

# Add compile definitions for miniz
# Don't define MINIZ_NO_ARCHIVE_APIS to enable ZIP archive APIs

# --- Define the Main CLI Executable ---
add_executable(chtl CLI/main.cpp)
target_link_libraries(chtl PRIVATE chtl_lib)
# Don't define MINIZ_NO_ARCHIVE_APIS to enable ZIP archive APIs
set_target_properties(chtl PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

# --- Define the Test Executables ---
# Enable testing
enable_testing()

# Get all test main.cpp files
file(GLOB TEST_FILES "Test/*/main.cpp")

foreach(test_file ${TEST_FILES})
    # Get the directory name of the test (e.g., "ListenTest")
    get_filename_component(test_name ${test_file} DIRECTORY)
    get_filename_component(test_name ${test_name} NAME)

    # Add an executable for each test
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE chtl_lib)
    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

    # Add a CTest test case
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
