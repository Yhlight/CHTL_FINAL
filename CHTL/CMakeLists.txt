cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找第三方库
find_package(PkgConfig REQUIRED)

# 查找ANTLR4 (可选)
find_package(antlr4-runtime QUIET)

# 查找libcss (可选)
pkg_check_modules(LIBCSS QUIET libcss)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty)

# 源文件
set(UTIL_SOURCES
    Util/FileSystem/FileSystem.cpp
    Util/FileSystem/FileSystem.h
    Util/ZipUtil/ZipUtil.cpp
    Util/ZipUtil/ZipUtil.h
    Util/StringUtil/StringUtil.cpp
    Util/StringUtil/StringUtil.h
)

set(SCANNER_SOURCES
    Scanner/UnifiedScanner.cpp
    Scanner/UnifiedScanner.h
)

set(CHTL_SOURCES
    CHTL/CHTLLexer/CHTLLexer.cpp
    CHTL/CHTLLexer/CHTLLexer.h
    CHTL/CHTLLexer/Token.cpp
    CHTL/CHTLLexer/Token.h
    CHTL/CHTLParser/CHTLParser.cpp
    CHTL/CHTLParser/CHTLParser.h
    CHTL/CHTLGenerator/CHTLGenerator.cpp
    CHTL/CHTLGenerator/CHTLGenerator.h
    CHTL/CHTLNode/BaseNode.cpp
    CHTL/CHTLNode/BaseNode.h
    CHTL/CHTLNode/ElementNode.cpp
    CHTL/CHTLNode/ElementNode.h
    CHTL/CHTLNode/TextNode.cpp
    CHTL/CHTLNode/TextNode.h
    CHTL/CHTLNode/StyleNode.cpp
    CHTL/CHTLNode/StyleNode.h
    CHTL/CHTLNode/TemplateNode.cpp
    CHTL/CHTLNode/TemplateNode.h
    CHTL/CHTLNode/CustomNode.cpp
    CHTL/CHTLNode/CustomNode.h
    CHTL/CHTLNode/ExpressionNode.cpp
    CHTL/CHTLNode/ExpressionNode.h
    CHTL/CHTLTemplate/TemplateManager.cpp
    CHTL/CHTLTemplate/TemplateManager.h
)

set(CHTL_JS_SOURCES
    # 暂时为空，后续添加
    # 添加一个空的源文件以避免CMake错误
    CHTL/CHTLLexer/Token.cpp
)

set(COMPILER_SOURCES
    CompilerDispatcher/CompilerDispatcher.cpp
    CompilerDispatcher/CompilerDispatcher.h
)

# 创建库
add_library(CHTL_UTIL ${UTIL_SOURCES})
add_library(CHTL_SCANNER ${SCANNER_SOURCES})
add_library(CHTL_CORE ${CHTL_SOURCES})
add_library(CHTL_JS_CORE ${CHTL_JS_SOURCES})
add_library(CHTL_COMPILER ${COMPILER_SOURCES})

# 链接库
target_link_libraries(CHTL_UTIL)
target_link_libraries(CHTL_SCANNER CHTL_UTIL)
target_link_libraries(CHTL_CORE CHTL_UTIL CHTL_SCANNER)
target_link_libraries(CHTL_JS_CORE CHTL_UTIL CHTL_SCANNER)
target_link_libraries(CHTL_COMPILER CHTL_CORE CHTL_JS_CORE CHTL_SCANNER CHTL_UTIL)

# 链接第三方库
if(antlr4-runtime_FOUND)
    target_link_libraries(CHTL_COMPILER antlr4-runtime)
endif()

if(LIBCSS_FOUND)
    target_link_libraries(CHTL_COMPILER ${LIBCSS_LIBRARIES})
endif()

# 创建可执行文件
add_executable(chtl main.cpp)
target_link_libraries(chtl CHTL_COMPILER)

# 测试
enable_testing()
# add_subdirectory(Test)  # 暂时注释掉，因为Test目录还没有CMakeLists.txt

# 安装
install(TARGETS chtl DESTINATION bin)
install(TARGETS CHTL_UTIL CHTL_SCANNER CHTL_CORE CHTL_JS_CORE CHTL_COMPILER DESTINATION lib)