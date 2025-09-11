# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(CHTL_Compiler CXX)

# Set the C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gather all your source files
set(COMPILER_SOURCES
    main.cpp
    Util/StringUtil/StringUtil.cpp
    Util/FileSystem/FileSystemUtil.cpp
    CHTL/CHTLLexer/Token.cpp
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp
    CHTL/CHTLEvaluator/Evaluator.cpp
    CHTL/CHTLStore/TemplateStore.cpp
    CHTL/CHTLLoader/CHTLLoader.cpp
)

# Add an executable target
add_executable(chtl_compiler ${COMPILER_SOURCES})

# Include directories for header files
# This allows us to use #include <StringUtil.h> for example
target_include_directories(chtl_compiler PUBLIC
    "${PROJECT_SOURCE_DIR}/CHTL"
    "${PROJECT_SOURCE_DIR}/Util"
    "${PROJECT_SOURCE_DIR}/CHTL/CHTLEvaluator"
    "${PROJECT_SOURCE_DIR}/CHTL/CHTLStore"
    "${PROJECT_SOURCE_DIR}/CHTL/CHTLLoader"
)
