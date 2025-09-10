cmake_minimum_required(VERSION 3.10)
project(CHTLResponsiveValueTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 源文件
set(SOURCES
    test_responsive_values.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSLexer.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSToken.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSGlobalMap.cpp
    CHTL_JS/CHTLJSParser/CHTLJSParser.cpp
    CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.cpp
    CHTL_JS/CHTLJSNode/CHTLJSBaseNode.cpp
)

# 创建可执行文件
add_executable(test_responsive_values ${SOURCES})

# 链接库
target_link_libraries(test_responsive_values)