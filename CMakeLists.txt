cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files for the parser
set(PARSER_SOURCES
    CHTL/CHTLParser/CHTLParser.cpp
)

set(LEXER_SOURCES
    CHTL/CHTLLexer/CHTLLexer.cpp
)

set(GENERATOR_SOURCES
    CHTL/CHTLGenerator/CHTLGenerator.cpp
)

# Main compiler executable
add_executable(chtl src/main.cpp ${LEXER_SOURCES} ${PARSER_SOURCES} ${GENERATOR_SOURCES})

# --- Testing ---
# Add Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Define a single test executable
add_executable(run_tests
    Test/LexerTest.cpp
    Test/ParserTest.cpp
    Test/GeneratorTest.cpp
    ${LEXER_SOURCES}
    ${PARSER_SOURCES}
    ${GENERATOR_SOURCES}
)

# Link GTest to the test executable
target_link_libraries(run_tests GTest::gtest_main)

# Add the test to CTest
include(GoogleTest)
gtest_discover_tests(run_tests)
