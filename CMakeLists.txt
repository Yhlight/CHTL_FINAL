cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# 查找依赖
find_package(PkgConfig REQUIRED)

# 第三方库
# ANTLR4 for CSS/JS compilation
find_package(antlr4-runtime REQUIRED)

# 源文件
set(CHTL_SOURCES
    src/CHTL/CHTLContext.cpp
    src/CHTL/CHTLGenerator.cpp
    src/CHTL/CHTLLexer.cpp
    src/CHTL/CHTLLoader.cpp
    src/CHTL/CHTLManage.cpp
    src/CHTL/CHTLParser.cpp
    src/CHTL/CHTLState.cpp
    src/CHTL/CHTLIOStream.cpp
    src/CHTL/CMODSystem.cpp
    src/CHTL/CHTLNode.cpp
    src/CHTL/TemplateNode.cpp
    src/CHTL/CustomNode.cpp
    src/CHTL/ImportNode.cpp
    src/CHTL/ModuleSystem.cpp
    src/CHTL/CLITools.cpp
    src/CHTL/CompilationMonitor.cpp
    src/CHTL/ConfigurationSystem.cpp
    src/CHTL/UseStatement.cpp
    src/CHTL/CHTLJSCompiler.cpp
    src/CHTL/ProjectWorkflow.cpp
    src/CHTL/SyntaxBoundaries.cpp
    src/CHTL/CJMODAPI.cpp
    
    src/CHTLJS/CHTLJSContext.cpp
    src/CHTLJS/CHTLJSGenerator.cpp
    src/CHTLJS/CHTLJSLexer.cpp
    src/CHTLJS/CHTLJSLoader.cpp
    src/CHTLJS/CHTLJSManage.cpp
    src/CHTLJS/CHTLJSParser.cpp
    src/CHTLJS/CHTLJSState.cpp
    src/CHTLJS/CHTLJSIOStream.cpp
    src/CHTLJS/CJMODSystem.cpp
    
    src/Scanner/UnifiedScanner.cpp
    src/CodeMerger/CodeMerger.cpp
    src/CompilerDispatcher/CompilerDispatcher.cpp
    
    src/Util/FileSystem/FileSystem.cpp
    src/Util/ZipUtil/ZipUtil.cpp
    src/Util/StringUtil/StringUtil.cpp
    
    src/Test/UtilTest/UtilTest.cpp
    src/Test/TokenTest/TokenTest.cpp
    src/Test/AstTest/AstTest.cpp
)

# 头文件
set(CHTL_HEADERS
    include/CHTL/CHTLContext.h
    include/CHTL/CHTLGenerator.h
    include/CHTL/CHTLLexer.h
    include/CHTL/CHTLLoader.h
    include/CHTL/CHTLManage.h
    include/CHTL/CHTLParser.h
    include/CHTL/CHTLState.h
    include/CHTL/CHTLIOStream.h
    include/CHTL/CMODSystem.h
    
    include/CHTLJS/CHTLJSContext.h
    include/CHTLJS/CHTLJSGenerator.h
    include/CHTLJS/CHTLJSLexer.h
    include/CHTLJS/CHTLJSLoader.h
    include/CHTLJS/CHTLJSManage.h
    include/CHTLJS/CHTLJSParser.h
    include/CHTLJS/CHTLJSState.h
    include/CHTLJS/CHTLJSIOStream.h
    include/CHTLJS/CJMODSystem.h
    
    include/Scanner/UnifiedScanner.h
    include/CodeMerger/CodeMerger.h
    include/CompilerDispatcher/CompilerDispatcher.h
    
    include/Util/FileSystem/FileSystem.h
    include/Util/ZipUtil/ZipUtil.h
    include/Util/StringUtil/StringUtil.h
    
    include/Test/UtilTest/UtilTest.h
    include/Test/TokenTest/TokenTest.h
    include/Test/AstTest/AstTest.h
)

# 创建库
add_library(chtl_lib STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})

# 链接库
target_link_libraries(chtl_lib 
    antlr4-runtime
)

# 创建可执行文件
add_executable(chtl src/main.cpp)
target_link_libraries(chtl chtl_lib)

# 创建测试程序
add_executable(test_compiler test_compiler.cpp)
target_link_libraries(test_compiler chtl_lib)

# 测试
enable_testing()
add_test(NAME chtl_tests COMMAND test_compiler)

# 安装
install(TARGETS chtl DESTINATION bin)
install(TARGETS chtl_lib DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)