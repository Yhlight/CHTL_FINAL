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
include_directories(include)

# 源文件
set(CHTL_SOURCES
    src/CHTL/CHTLContext.cpp
    src/CHTL/CHTLGenerator.cpp
    src/CHTL/CHTLLexer.cpp
    src/CHTL/CHTLNode.cpp
    src/CHTL/CHTLParser.cpp
    src/Scanner/UnifiedScanner.cpp
    src/CHTL_JS/CHTLJSLexer.cpp
    src/CHTL_JS/CHTLJSParser.cpp
    src/CHTL_JS/CHTLJSGenerator.cpp
    src/Compiler/CompilerDispatcher.cpp
    src/Compiler/CodeMerger.cpp
    src/Module/CMODSystem.cpp
    src/Module/CJMODSystem.cpp
    src/CLI/CHTLCli.cpp
)

# 头文件
set(CHTL_HEADERS
    include/CHTL/CHTLContext.h
    include/CHTL/CHTLGenerator.h
    include/CHTL/CHTLLexer.h
    include/CHTL/CHTLNode.h
    include/CHTL/CHTLParser.h
    include/Scanner/UnifiedScanner.h
    include/CHTL_JS/CHTLJSLexer.h
    include/CHTL_JS/CHTLJSParser.h
    include/CHTL_JS/CHTLJSGenerator.h
    include/Compiler/CompilerDispatcher.h
    include/Compiler/CodeMerger.h
    include/Module/CMODSystem.h
    include/Module/CJMODSystem.h
    include/CLI/CHTLCli.h
)

# 创建库
add_library(chtl_lib STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})

# 创建可执行文件
add_executable(chtl src/main.cpp)
target_link_libraries(chtl chtl_lib)

# 创建CLI可执行文件
add_executable(chtl_cli src/cli_main.cpp)
target_link_libraries(chtl_cli chtl_lib)

# 创建测试可执行文件
add_executable(test_compiler test_compiler.cpp)
target_link_libraries(test_compiler chtl_lib)

# 测试
enable_testing()
add_subdirectory(test)

# 安装
install(TARGETS chtl chtl_lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)