cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create a library for the compiler components
add_library(chtl_lib
    src/CHTL/Lexer/Lexer.cpp
    src/CHTL/Parser/Parser.cpp
    src/CHTL/Generator/Generator.cpp
    src/CHTL/Context/TemplateRegistry.cpp
)
# We need to tell the library where to find its headers
target_include_directories(chtl_lib PUBLIC src)


# Create the test executable
add_executable(CompilerTest
    src/Test/CompilerTest.cpp
    src/Test/AstPrinter.cpp
)
target_link_libraries(CompilerTest chtl_lib)


# Create the main CLI executable
add_executable(chtl-compiler main.cpp)
target_link_libraries(chtl-compiler chtl_lib)
