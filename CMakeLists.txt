cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Define the executable
add_executable(chtl_compiler src/main.cpp)

# Define the source files for the CHTL library
set(CHTL_LIB_SOURCES
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLParser/CHTLParser.cpp
    src/CHTL/CHTLGenerator/CHTLGenerator.cpp
    src/CHTL/CHTLLoader/CHTLLoader.cpp
    src/Scanner/UnifiedScanner.cpp
    src/CompilerDispatcher/CompilerDispatcher.cpp
    src/CHTLJS/CHTLJSLexer.cpp
    src/CHTLJS/CHTLJSParser.cpp
    src/CHTLJS/CHTLJSGenerator.cpp
)

# Create a static library for our compiler components
add_library(chtl_lib STATIC ${CHTL_LIB_SOURCES})

# Link the executable against our library
target_link_libraries(chtl_compiler PRIVATE chtl_lib)

# Add the src directory to the include path for the executable and the library
target_include_directories(chtl_compiler PUBLIC src)
target_include_directories(chtl_lib PUBLIC src)

# --- Test Suite ---
# Add a test for each .chtl file in the root directory.
file(GLOB CHTL_TEST_FILES "*.chtl")

foreach(test_file ${CHTL_TEST_FILES})
    string(REPLACE "./" "" test_name ${test_file})
    string(REPLACE "./" "" test_name ${test_name})

    add_test(
        NAME run_${test_name}
        COMMAND chtl_compiler ${test_file}
    )
endforeach()
