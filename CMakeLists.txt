cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Header Include Directories ---
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# --- Compiler Sources ---
# We gather all .cpp files from the relevant directories.
file(GLOB_RECURSE COMPILER_SOURCES
    "CHTL/*.cpp"
    "CHTL/CHTLLexer/*.cpp"
    "CHTL/CHTLParser/*.cpp"
    "CHTL/CHTLGenerator/*.cpp"
    "CHTL/CHTLAnalyzer/*.cpp"
    "CompilerDispatcher/*.cpp"
    "CodeMerger/*.cpp"
    "Scanner/*.cpp"
    "Util/FileSystem/*.cpp"
    "Util/ZipUtil/*.cpp"
)

# Add the miniz C source file to our list of sources.
list(APPEND COMPILER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/miniz.c")

# --- Compiler Executable ---
add_executable(chtl main.cpp ${COMPILER_SOURCES})

# --- Test Sources ---
file(GLOB TEST_MAIN "Test/TestMain.cpp")
file(GLOB_RECURSE TEST_FILES "Test/*.cpp")

# --- Test Executable ---
# The test executable needs to be linked against the compiler sources as well.
add_executable(run_tests ${TEST_MAIN} ${TEST_FILES} ${COMPILER_SOURCES})

# Add a custom target to make running tests easier.
# e.g., `make test`
add_custom_target(test
    COMMAND ./run_tests
    DEPENDS run_tests
)

# Print a message to the user after configuration.
message(STATUS "Configuration complete. Run 'make' to build.")
message(STATUS "To build the compiler, run: make chtl")
message(STATUS "To build the tests, run:    make run_tests")
message(STATUS "To run the tests, run:      make test")
