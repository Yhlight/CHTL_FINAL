cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# 设置调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})

# 源文件
set(CHTL_SOURCES
    main.cpp
    CLI/CHTLCli.cpp
    CHTL/CMODSystem/CMODSystem.cpp
    "CHTL JS/CHTLJSContext/CHTLJSContext.cpp"
    "CHTL JS/CHTLJSLexer/Token.cpp"
    "CHTL JS/CHTLJSLexer/CHTLJSLexer.cpp"
    "CHTL JS/CHTLJSNode/CHTLJSBaseNode.cpp"
    "CHTL JS/CHTLJSNode/CHTLJSFunctionNode.cpp"
    "CHTL JS/CHTLJSNode/CHTLJSSpecialNode.cpp"
    "CHTL JS/CHTLJSParser/CHTLJSParser.cpp"
    "CHTL JS/CHTLJSGenerator/CHTLJSGenerator.cpp"
    "CHTL JS/CJMODSystem/CJMODAPI.cpp"
    Scanner/UnifiedScanner.cpp
)

# 头文件
set(CHTL_HEADERS
    CLI/CHTLCli.h
    CHTL/CMODSystem/CMODSystem.h
    "CHTL JS/CHTLJSContext/CHTLJSContext.h"
    "CHTL JS/CHTLJSLexer/Token.h"
    "CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
    "CHTL JS/CHTLJSNode/CHTLJSBaseNode.h"
    "CHTL JS/CHTLJSNode/CHTLJSFunctionNode.h"
    "CHTL JS/CHTLJSNode/CHTLJSSpecialNode.h"
    "CHTL JS/CHTLJSParser/CHTLJSParser.h"
    "CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"
    "CHTL JS/CJMODSystem/CJMODAPI.h"
    Scanner/UnifiedScanner.h
)

# 创建可执行文件
add_executable(chtl ${CHTL_SOURCES} ${CHTL_HEADERS})

# 设置输出名称
set_target_properties(chtl PROPERTIES OUTPUT_NAME "chtl")

# 链接库
target_link_libraries(chtl)

# 安装规则
install(TARGETS chtl DESTINATION bin)
install(FILES ${CHTL_HEADERS} DESTINATION include/chtl)

# 创建测试目标
enable_testing()

# 添加测试
add_test(NAME chtl_version COMMAND chtl version)
add_test(NAME chtl_help COMMAND chtl help)

# 创建示例
add_custom_target(example
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/example
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/example.chtl ${CMAKE_BINARY_DIR}/example/
    COMMAND chtl compile ${CMAKE_BINARY_DIR}/example/example.chtl -o ${CMAKE_BINARY_DIR}/example/example.html
    DEPENDS chtl
    COMMENT "Creating example"
)

# 创建文档目标
add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "Documentation generation not implemented yet"
    COMMENT "Generating documentation"
)

# 设置编译特性
target_compile_features(chtl PRIVATE cxx_std_17)

# 添加编译定义
target_compile_definitions(chtl PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# 设置包含目录
target_include_directories(chtl PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/CLI
    ${CMAKE_SOURCE_DIR}/CHTL
    ${CMAKE_SOURCE_DIR}/CHTL/CMODSystem
    ${CMAKE_SOURCE_DIR}/CHTL JS
    ${CMAKE_SOURCE_DIR}/CHTL JS/CHTLJSContext
    ${CMAKE_SOURCE_DIR}/CHTL JS/CHTLJSLexer
    ${CMAKE_SOURCE_DIR}/CHTL JS/CHTLJSNode
    ${CMAKE_SOURCE_DIR}/CHTL JS/CHTLJSParser
    ${CMAKE_SOURCE_DIR}/CHTL JS/CHTLJSGenerator
    ${CMAKE_SOURCE_DIR}/CHTL JS/CJMODSystem
    ${CMAKE_SOURCE_DIR}/Scanner
)

# 创建包
set(CPACK_PACKAGE_NAME "CHTL")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION "C++ HyperText Language")
set(CPACK_PACKAGE_VENDOR "CHTL Team")
set(CPACK_PACKAGE_CONTACT "contact@chtl.dev")

include(CPack)