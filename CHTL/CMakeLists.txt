cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set include directory for the whole project
include_directories(src)

# --- Source File Definitions ---

set(COMPILER_SOURCES
    src/CHTL/CHTLLexer/Lexer.cpp
    src/CHTL/CHTLParser/Parser.cpp
    src/CHTL/CHTLGenerator/Generator.cpp
    src/CHTL/Evaluator/ExprStringifier.cpp
    src/CHTL/Evaluator/Evaluator.cpp
    src/CHTL/CHTLContext/CHTLContext.cpp
    src/CHTL/CHTLLoader/CHTLLoader.cpp # Add the new loader
)

# --- Executable Target Definitions ---

add_executable(chtl src/main.cpp ${COMPILER_SOURCES})
add_executable(run_tests src/Test/run_tests.cpp ${COMPILER_SOURCES})

# Optional: Add a custom test target for integration with CTest
add_test(NAME CHTL_Unit_Tests COMMAND run_tests)
