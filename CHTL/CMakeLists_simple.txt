cmake_minimum_required(VERSION 3.10)
project(CHTL_Simple)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})

# CHTL核心源文件
set(CHTL_CORE_SOURCES
    CHTL/CHTLLexer/CHTLLexer_impl.cpp
    CHTL/CHTLLexer/CHTLLexer_scanner.cpp
    CHTL/CHTLLexer/CHTLLexer_operators.cpp
    CHTL/CHTLLexer/CHTLLexer_main.cpp
    CHTL/CHTLParser/CHTLParser_impl.cpp
    CHTL/CHTLParser/CHTLParser_parse.cpp
    CHTL/CHTLParser/CHTLParser_helpers.cpp
    CHTL/CHTLParser/CHTLParser_advanced.cpp
    CHTL/CHTLParser/CHTLParser_management.cpp
    CHTL/CHTLGenerator/CHTLGenerator.cpp
)

# 创建CHTL核心库
add_library(chtl_core_simple ${CHTL_CORE_SOURCES})

# 创建简单测试程序
add_executable(simple_test simple_test.cpp)
target_link_libraries(simple_test chtl_core_simple)

add_executable(debug_peek debug_peek.cpp)
target_link_libraries(debug_peek chtl_core_simple)

# 设置输出目录
set_target_properties(simple_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(debug_peek PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 创建输出目录
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 打印配置信息
message(STATUS "CHTL简单项目配置:")
message(STATUS "  C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "  编译选项: ${CMAKE_CXX_FLAGS}")
message(STATUS "  核心源文件数量: ${CMAKE_CURRENT_LIST_LENGTH} ${CHTL_CORE_SOURCES}")
message(STATUS "  输出目录: ${CMAKE_BINARY_DIR}/bin")