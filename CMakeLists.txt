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
# 暂时注释掉ANTLR4依赖，使用内置实现
# find_package(antlr4-runtime REQUIRED)

# 源文件
set(CHTL_SOURCES
    src/CHTL/CHTLContext.cpp
    src/CHTL/CHTLLexer.cpp
    src/CHTL/CHTLParser.cpp
    src/CHTL/CHTLNode.cpp
    src/CHTL/TemplateNode.cpp
    src/CHTL/CustomNode.cpp
    src/CHTL/ImportNode.cpp
    src/CHTL/ModuleSystem.cpp
    src/Scanner/UnifiedScanner.cpp
    src/CHTL/CLITools.cpp
    src/CHTL/CompilationMonitor.cpp
    src/CHTL/ConfigurationSystem.cpp
    src/CHTL/UseStatement.cpp
    src/CHTL/CHTLJSCompiler.cpp
    src/CHTL/ProjectWorkflow.cpp
    src/CHTL/SyntaxBoundaries.cpp
    src/CHTL/CJMODAPI.cpp
    src/CHTL/OfficialModules.cpp
    src/CHTL/WildcardImport.cpp
    src/CHTL/DefaultStructGenerator.cpp
    src/CHTL/CompilationMonitor.cpp
    src/CHTL/VSCodeExtension.cpp
    src/CHTL/ConcreteNodes.cpp
    
    src/CHTLJS/CJMODSystem.cpp
    
    src/Scanner/UnifiedScanner.cpp
)

# 头文件
set(CHTL_HEADERS
    include/CHTL/CHTLContext.h
    include/CHTL/CHTLLexer.h
    include/CHTL/CHTLParser.h
    include/CHTL/CHTLNode.h
    include/CHTL/TemplateNode.h
    include/CHTL/CustomNode.h
    include/CHTL/ImportNode.h
    include/CHTL/ModuleSystem.h
    include/CHTL/CLITools.h
    include/CHTL/CompilationMonitor.h
    include/CHTL/ConfigurationSystem.h
    include/CHTL/UseStatement.h
    include/CHTL/CHTLJSCompiler.h
    include/CHTL/DefaultStructGenerator.h
    include/CHTL/OfficialModules.h
    include/CHTL/ProjectWorkflow.h
    include/CHTL/SyntaxBoundaries.h
    include/CHTL/VSCodeExtension.h
    include/CHTL/WildcardImport.h
    include/CHTL/CJMODAPI.h
    include/CHTL/ConcreteNodes.h
    include/CHTLJS/CHTLJSContext.h
    include/CHTLJS/CJMODSystem.h
    include/Scanner/UnifiedScanner.h
)

# 创建库
add_library(chtl_lib STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})

# 链接库
# target_link_libraries(chtl_lib 
#     antlr4-runtime
# )

# 创建可执行文件
add_executable(chtl src/main.cpp)
target_link_libraries(chtl chtl_lib)

add_executable(chtl_compiler src/chtl_compiler.cpp)
target_link_libraries(chtl_compiler chtl_lib)

add_executable(simple_chtl_compiler src/simple_chtl_compiler.cpp)

add_executable(improved_chtl_compiler src/improved_chtl_compiler.cpp)

add_executable(working_chtl_compiler src/working_chtl_compiler.cpp)

add_executable(chtl_js_compiler src/chtl_js_compiler.cpp)

add_executable(chtl_cli src/chtl_cli.cpp)

add_executable(enhanced_template_compiler src/enhanced_template_compiler.cpp)

add_executable(module_system_compiler src/module_system_compiler.cpp)

add_executable(error_handler src/error_handler.cpp)

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