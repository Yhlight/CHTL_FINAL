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

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# 查找依赖
find_package(PkgConfig REQUIRED)

# 查找ANTLR4 (用于CSS和JS编译器) - 暂时注释掉
# find_package(antlr4-runtime REQUIRED)

# 查找其他依赖
find_package(Threads REQUIRED)

# 源文件
set(CHTL_SOURCES
    src/CHTL/CHTLContext.cpp
    src/CHTL/CHTLGenerator.cpp
    src/CHTL/CHTLLexer.cpp
    src/CHTL/CHTLLoader.cpp
    src/CHTL/CHTLManage.cpp
    src/CHTL/CHTLNode.cpp
    src/CHTL/CHTLParser.cpp
    src/CHTL/CHTLState.cpp
    src/CHTL/CHTLIOStream.cpp
    src/CHTL/CMODSystem.cpp
    src/CHTL/TemplateSystem.cpp
    src/CHTL/StyleSystem.cpp
    src/CHTL_JS/CHTLJSContext.cpp
    src/CHTL_JS/CHTLJSGenerator.cpp
    src/CHTL_JS/CHTLJSLexer.cpp
    src/CHTL_JS/CHTLJSLoader.cpp
    src/CHTL_JS/CHTLJSManage.cpp
    src/CHTL_JS/CHTLJSNode.cpp
    src/CHTL_JS/CHTLJSParser.cpp
    src/CHTL_JS/CHTLJSState.cpp
    src/CHTL_JS/CHTLJSIOStream.cpp
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
    src/CHTL/CHTLManage.h
    src/CHTL/CHTLNode.h
    src/CHTL/CHTLParser.h
    src/CHTL/CHTLState.h
    src/CHTL/CHTLIOStream.h
    src/CHTL/CMODSystem.h
    src/CHTL/TemplateSystem.h
    src/CHTL/StyleSystem.h
    src/CHTL_JS/CHTLJSContext.h
    src/CHTL_JS/CHTLJSGenerator.h
    src/CHTL_JS/CHTLJSLexer.h
    src/CHTL_JS/CHTLJSLoader.h
    src/CHTL_JS/CHTLJSManage.h
    src/CHTL_JS/CHTLJSNode.h
    src/CHTL_JS/CHTLJSParser.h
    src/CHTL_JS/CHTLJSState.h
    src/CHTL_JS/CHTLJSIOStream.h
    src/CHTL_JS/CJMODSystem.h
    src/Scanner/UnifiedScanner.h
    src/CodeMerger/CodeMerger.h
    src/CompilerDispatcher/CompilerDispatcher.h
    src/Util/FileSystem/FileSystem.h
    src/Util/ZipUtil/ZipUtil.h
    src/Util/StringUtil/StringUtil.h
)

# 创建CHTL库
add_library(chtl_lib STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})
target_link_libraries(chtl_lib 
    # antlr4-runtime 
    Threads::Threads
)

# 创建CHTL编译器可执行文件
add_executable(chtl_compiler src/main.cpp)
target_link_libraries(chtl_compiler chtl_lib)

# 创建测试可执行文件
add_executable(chtl_test src/Test/main.cpp)
target_link_libraries(chtl_test chtl_lib)

# 安装规则
install(TARGETS chtl_compiler chtl_test
    RUNTIME DESTINATION bin
)

install(TARGETS chtl_lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 复制模块到安装目录
install(DIRECTORY Module/ DESTINATION share/chtl/module)