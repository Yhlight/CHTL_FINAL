cmake_minimum_required(VERSION 3.10)
project(CHTL)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})

# CHTL JS 源文件
set(CHTLJS_SOURCES
    CHTL/CHTLJS/CHTLJS.cpp
    CHTL/CHTLJS/CHTLJSContext/CHTLJSContext.cpp
    CHTL/CHTLJS/CHTLJSLexer/CHTLJSLexer.cpp
    CHTL/CHTLJS/CHTLJSLexer/Token.cpp
    CHTL/CHTLJS/CHTLJSParser/CHTLJSParser.cpp
    CHTL/CHTLJS/CHTLJSParser/ASTNode.cpp
    CHTL/CHTLJS/CHTLJSGenerator/CHTLJSGenerator.cpp
)

# CHTL 源文件
set(CHTL_SOURCES
    CHTL/CHTL/CHTLLexer/CHTLLexer.cpp
    CHTL/CHTL/CHTLLexer/CHTLToken.cpp
    CHTL/CHTL/CHTLParser/CHTLParser.cpp
    CHTL/CHTL/CHTLParser/CHTLASTNode.cpp
    CHTL/CHTL/CHTLGenerator/CHTLGenerator.cpp
)

# 统一扫描器源文件
set(SCANNER_SOURCES
    Scanner/UnifiedScanner.cpp
)

# 编译器调度器源文件
set(COMPILER_DISPATCHER_SOURCES
    CompilerDispatcher/CompilerDispatcher.cpp
)

# 代码合并器源文件
set(CODE_MERGER_SOURCES
    CodeMerger/CodeMerger.cpp
)

# 创建库
add_library(chtljs STATIC ${CHTLJS_SOURCES})
add_library(chtl STATIC ${CHTL_SOURCES})
add_library(scanner STATIC ${SCANNER_SOURCES})
add_library(compiler_dispatcher STATIC ${COMPILER_DISPATCHER_SOURCES})
add_library(code_merger STATIC ${CODE_MERGER_SOURCES})

# 测试可执行文件
add_executable(test_chtljs test_chtljs.cpp)
target_link_libraries(test_chtljs chtljs)

add_executable(test_basic test_basic.cpp)

add_executable(demo demo.cpp)
target_link_libraries(demo chtljs chtl scanner compiler_dispatcher code_merger)

# 安装目标
install(TARGETS chtljs test_chtljs
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装头文件
install(DIRECTORY CHTL/CHTLJS/
    DESTINATION include/CHTL/CHTLJS
    FILES_MATCHING PATTERN "*.h"
)