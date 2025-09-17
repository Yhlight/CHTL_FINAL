cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# 查找依赖
find_package(PkgConfig REQUIRED)

# 第三方库
# ANTLR4 for CSS/JS parsing (可选)
find_package(antlr4-runtime QUIET)

# 源文件
set(CHTL_SOURCES
    src/main.cpp
    src/CHTL/CHTLContext.cpp
    src/CHTL/CHTLGenerator.cpp
    src/CHTL/CHTLLexer.cpp
    src/CHTL/CHTLLoader.cpp
    src/CHTL/CHTLParser.cpp
    src/CHTL/CHTLState.cpp
    src/CHTL/CHTLNode/BaseNode.cpp
    src/CHTL/CHTLNode/ElementNode.cpp
    src/CHTL/CHTLNode/TextNode.cpp
    src/CHTL/CHTLNode/TemplateNode.cpp
    src/CHTL/CHTLNode/CustomNode.cpp
    src/CHTL/CHTLNode/StyleNode.cpp
    src/CHTL/CHTLNode/ScriptNode.cpp
    src/CHTL/CHTLNode/OriginNode.cpp
    src/CHTL/CHTLNode/ImportNode.cpp
    src/CHTL/CHTLNode/ConfigNode.cpp
    src/CHTL/CHTLNode/NamespaceNode.cpp
    src/CHTL/CHTLNode/OperatorNode.cpp
    src/CHTL/CMODSystem.cpp
    src/CHTL_JS/CHTLJSContext.cpp
    src/CHTL_JS/CHTLJSGenerator.cpp
    src/CHTL_JS/CHTLJSLexer.cpp
    src/CHTL_JS/CHTLJSLoader.cpp
    src/CHTL_JS/CHTLJSParser.cpp
    src/CHTL_JS/CHTLJSState.cpp
    src/CHTL_JS/CHTLJSNode/CHTLJSBaseNode.cpp
    src/CHTL_JS/CJMODSystem.cpp
    src/Scanner/UnifiedScanner.cpp
    src/CodeMerger/CodeMerger.cpp
    src/CompilerDispatcher/CompilerDispatcher.cpp
    src/Util/FileSystem/FileSystem.cpp
    src/Util/ZipUtil/ZipUtil.cpp
    src/Util/StringUtil/StringUtil.cpp
)

# 头文件
set(CHTL_HEADERS
    src/CHTL/CHTLContext.h
    src/CHTL/CHTLGenerator.h
    src/CHTL/CHTLLexer.h
    src/CHTL/CHTLLoader.h
    src/CHTL/CHTLParser.h
    src/CHTL/CHTLState.h
    src/CHTL/CHTLNode/BaseNode.h
    src/CHTL/CHTLNode/ElementNode.h
    src/CHTL/CHTLNode/TextNode.h
    src/CHTL/CHTLNode/TemplateNode.h
    src/CHTL/CHTLNode/CustomNode.h
    src/CHTL/CHTLNode/StyleNode.h
    src/CHTL/CHTLNode/ScriptNode.h
    src/CHTL/CHTLNode/OriginNode.h
    src/CHTL/CHTLNode/ImportNode.h
    src/CHTL/CHTLNode/ConfigNode.h
    src/CHTL/CHTLNode/NamespaceNode.h
    src/CHTL/CHTLNode/OperatorNode.h
    src/CHTL/CMODSystem.h
    src/CHTL_JS/CHTLJSContext.h
    src/CHTL_JS/CHTLJSGenerator.h
    src/CHTL_JS/CHTLJSLexer.h
    src/CHTL_JS/CHTLJSLoader.h
    src/CHTL_JS/CHTLJSParser.h
    src/CHTL_JS/CHTLJSState.h
    src/CHTL_JS/CHTLJSNode/CHTLJSBaseNode.h
    src/CHTL_JS/CJMODSystem.h
    src/Scanner/UnifiedScanner.h
    src/CodeMerger/CodeMerger.h
    src/CompilerDispatcher/CompilerDispatcher.h
    src/Util/FileSystem/FileSystem.h
    src/Util/ZipUtil/ZipUtil.h
    src/Util/StringUtil/StringUtil.h
)

# 创建可执行文件
add_executable(chtl ${CHTL_SOURCES} ${CHTL_HEADERS})

# 链接库
target_link_libraries(chtl 
    ${CMAKE_THREAD_LIBS_INIT}
)

# 如果找到ANTLR4，则链接
if(antlr4-runtime_FOUND)
    target_link_libraries(chtl antlr4-runtime)
endif()

# 测试
enable_testing()
add_subdirectory(test)

# 安装
install(TARGETS chtl DESTINATION bin)