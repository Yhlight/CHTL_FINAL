cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(chtl_compiler main.cpp)

# Link libraries
target_link_libraries(chtl_compiler PRIVATE chtl_lexer chtl_ast chtl_parser chtl_generator chtl_js_lexer chtl_js_parser)

# Add subdirectories for organizing the project
add_subdirectory(CHTL)
add_subdirectory(CHTL_JS)
add_subdirectory(Scanner)
add_subdirectory(CodeMerger)
add_subdirectory(CompilerDispatcher)
add_subdirectory(Test)
