cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Gather all source files for the CHTL library component
set(CHTL_LIB_SOURCES
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp
)

# Define this as a static library
add_library(chtl_lib STATIC ${CHTL_LIB_SOURCES})

# Publicly expose the include directory for anything that links against chtl_lib
target_include_directories(chtl_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Test Executable ---
# Define the executable for our test driver
add_executable(ch-test Test/main.cpp)

# Link the test executable against our library
target_link_libraries(ch-test PRIVATE chtl_lib)
