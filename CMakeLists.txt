cmake_minimum_required(VERSION 3.10)
project(CHTL)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})

# 源文件
set(SOURCES
    main.cpp
    miniz.c
    CompilerDispatcher/CompilerDispatcher.cpp
    Scanner/UnifiedScanner.cpp
    "CHTL JS/CHTLJSContext/CHTLJSContext.cpp"
    "CHTL JS/CHTLJSLexer/Token.cpp"
    "CHTL JS/CHTLJSLexer/CHTLJSLexer.cpp"
    "CHTL JS/CHTLJSParser/ASTNode.cpp"
    "CHTL JS/CHTLJSParser/CHTLJSParser.cpp"
    "CHTL JS/CHTLJSGenerator/CHTLJSGenerator.cpp"
)

# 头文件
set(HEADERS
    CompilerDispatcher/CompilerDispatcher.h
    Scanner/UnifiedScanner.h
    "CHTL JS/CHTLJSContext/CHTLJSContext.h"
    "CHTL JS/CHTLJSLexer/Token.h"
    "CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
    "CHTL JS/CHTLJSParser/ASTNode.h"
    "CHTL JS/CHTLJSParser/CHTLJSParser.h"
    "CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"
    miniz.h
)

# 创建可执行文件
add_executable(chtl ${SOURCES} ${HEADERS})

# 设置输出目录
set_target_properties(chtl PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 如果是Debug模式，添加调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(chtl PRIVATE DEBUG_MODE=1)
endif()

# 安装规则
install(TARGETS chtl DESTINATION bin)
install(FILES ${HEADERS} DESTINATION include/chtl)

# 创建测试目标
enable_testing()

# 添加测试
add_test(NAME chtl_test COMMAND chtl test.chtl)