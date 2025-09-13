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
    src/CHTL/CHTLDebug/ASTPrinter.cpp
    src/Scanner/UnifiedScanner.cpp
    src/CompilerDispatcher/CompilerDispatcher.cpp
)

# Create a static library for our compiler components
add_library(chtl_lib STATIC ${CHTL_LIB_SOURCES})

# Link the executable against our library
target_link_libraries(chtl_compiler PRIVATE chtl_lib)

# Add the src directory to the include path for the executable and the library
target_include_directories(chtl_compiler PUBLIC src)
target_include_directories(chtl_lib PUBLIC src)

# Optional: Add custom test commands
add_test(
    NAME run_delete_test
    COMMAND chtl_compiler ${CMAKE_CURRENT_SOURCE_DIR}/delete_test.chtl
)
add_test(
    NAME run_insert_test
    COMMAND chtl_compiler ${CMAKE_CURRENT_SOURCE_DIR}/insert_test.chtl
)
