cmake_minimum_required(VERSION 3.10)
project(CHTLProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录（全局）
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/CHTL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/CHTL/CHTL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/CHTL_JS)

# 测试目标（保留原有）
set(TEST_SOURCES
    test_responsive_values.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSLexer.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSToken.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSGlobalMap.cpp
    CHTL_JS/CHTLJSParser/CHTLJSParser.cpp
    CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.cpp
    CHTL_JS/CHTLJSNode/CHTLJSBaseNode.cpp
)
add_executable(test_responsive_values ${TEST_SOURCES})

# 递归收集源码
file(GLOB_RECURSE CORE_COMPILER_DISP ${CMAKE_CURRENT_SOURCE_DIR}/CompilerDispatcher/*.cpp)
file(GLOB_RECURSE CORE_CODE_MERGER   ${CMAKE_CURRENT_SOURCE_DIR}/CodeMerger/*.cpp)
file(GLOB_RECURSE CORE_SCANNER       ${CMAKE_CURRENT_SOURCE_DIR}/Scanner/*.cpp)
file(GLOB_RECURSE CORE_CHTL          ${CMAKE_CURRENT_SOURCE_DIR}/CHTL/*.cpp)
file(GLOB_RECURSE CORE_CHTL_JS       ${CMAKE_CURRENT_SOURCE_DIR}/CHTL_JS/*.cpp)

set(CORE_SOURCES
    ${CORE_COMPILER_DISP}
    ${CORE_CODE_MERGER}
    ${CORE_SCANNER}
    ${CORE_CHTL}
    ${CORE_CHTL_JS}
)

# CLI 可执行文件
set(CLI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/CLI/CLITool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CLI/main.cpp
)
add_executable(chtl_cli ${CLI_SOURCES} ${CORE_SOURCES})
target_include_directories(chtl_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTL
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTL/CHTL
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTL_JS
)

# 编译器主程序
add_executable(chtl_compiler ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CORE_SOURCES})
target_include_directories(chtl_compiler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTL
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTL/CHTL
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTL_JS
)

# 链接库
target_link_libraries(test_responsive_values)