cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler)

# Set C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add an executable
add_executable(chtl_compiler main.cpp
    Scanner/CHTLUnifiedScanner.cpp
    CompilerDispatcher/CompilerDispatcher.cpp
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLNode/AstNodes.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp
    CHTL/LocalStyleParser/LocalStyleParser.cpp
    CHTL/ASTProcessors/StyleProcessor.cpp
)

# Include directories
target_include_directories(chtl_compiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Scanner
    ${CMAKE_CURRENT_SOURCE_DIR}/CompilerDispatcher
    ${CMAKE_CURRENT_SOURCE_DIR}/Util
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTL
)

# Add a custom target to run the compiler on a test file
# This ensures it always rebuilds if needed and then runs.
add_custom_target(run_compiler
    COMMAND ${CMAKE_BINARY_DIR}/chtl_compiler test.chtl
    DEPENDS chtl_compiler
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
