cmake_minimum_required(VERSION 3.10)

project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source files for the compiler library
set(COMPILER_LIB_SOURCES
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLNode/Node.cpp
    CHTL/CHTLNode/ElementNode.cpp
    CHTL/CHTLNode/TextNode.cpp
    CHTL/CHTLNode/StyleNode.cpp
    CHTL/CHTLNode/TemplateDefinitionNode.cpp
    CHTL/CHTLNode/TemplateUsageNode.cpp
    CHTL/CHTLNode/StylePropertyNode.cpp
    CHTL/CHTLNode/ElementBlockNode.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp
)

# Create a static library to link against
add_library(chtl_lib STATIC ${COMPILER_LIB_SOURCES})

# Main executable
add_executable(chtl CompilerDispatcher/main.cpp)
target_link_libraries(chtl PRIVATE chtl_lib)

# Enable testing with CTest
enable_testing()

# --- Lexer Test ---
add_executable(run_lexer_tests Test/LexerTest.cpp)
target_link_libraries(run_lexer_tests PRIVATE chtl_lib)
add_test(NAME LexerTests COMMAND run_lexer_tests)

# --- Parser Test ---
add_executable(run_parser_tests Test/ParserTest.cpp)
target_link_libraries(run_parser_tests PRIVATE chtl_lib)
add_test(NAME ParserTests COMMAND run_parser_tests)

# --- Generator Test ---
add_executable(run_generator_tests Test/GeneratorTest.cpp)
target_link_libraries(run_generator_tests PRIVATE chtl_lib)
add_test(NAME GeneratorTests COMMAND run_generator_tests)

# --- Attribute Test ---
add_executable(run_attribute_tests Test/AttributeTest.cpp)
target_link_libraries(run_attribute_tests PRIVATE chtl_lib)
add_test(NAME AttributeTests COMMAND run_attribute_tests)

# --- Style Test ---
add_executable(run_style_tests Test/StyleTest.cpp)
target_link_libraries(run_style_tests PRIVATE chtl_lib)
add_test(NAME StyleTests COMMAND run_style_tests)

# --- Error Test ---
add_executable(run_error_tests Test/ErrorTest.cpp)
target_link_libraries(run_error_tests PRIVATE chtl_lib)
add_test(NAME ErrorTests COMMAND run_error_tests)

# --- Template Test ---
add_executable(run_template_tests Test/TemplateTest.cpp)
target_link_libraries(run_template_tests PRIVATE chtl_lib)
add_test(NAME TemplateTests COMMAND run_template_tests)

# --- Element Template Test ---
add_executable(run_element_template_tests Test/ElementTemplateTest.cpp)
target_link_libraries(run_element_template_tests PRIVATE chtl_lib)
add_test(NAME ElementTemplateTests COMMAND run_element_template_tests)


# Add directories for header files
include_directories(
    CHTL/CHTLLexer
    CHTL/CHTLNode
    CHTL/CHTLParser
    CHTL/CHTLGenerator
    CompilerDispatcher
    Util
    Test
)
