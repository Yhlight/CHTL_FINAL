cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Define the executable
add_executable(chtl_compiler src/main.cpp)

# Define the source files for the CHTL library
set(CHTL_LIB_SOURCES
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLParser/CHTLParser.cpp
    src/CHTL/CHTLGenerator/CHTLGenerator.cpp
    src/CHTL/CHTLLoader/CHTLLoader.cpp
    src/CHTL/CHTLNode/ExpressionNode.cpp
)

# Create a static library for our compiler components
add_library(chtl_lib STATIC ${CHTL_LIB_SOURCES})

# Link the executable against our library
target_link_libraries(chtl_compiler PRIVATE chtl_lib)

# Add the src directory to the include path for the executable and the library
target_include_directories(chtl_compiler PUBLIC src)
target_include_directories(chtl_lib PUBLIC src)

# Add tests
add_executable(
    CHTL_tests
    tests/parser_test.cpp
)

target_link_libraries(
    CHTL_tests
    PRIVATE
    chtl_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(CHTL_tests)
