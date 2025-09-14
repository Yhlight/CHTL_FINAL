cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/CHTL)
include_directories(${CMAKE_SOURCE_DIR}/CHTL_JS)
include_directories(${CMAKE_SOURCE_DIR}/Scanner)
include_directories(${CMAKE_SOURCE_DIR}/CodeMerger)
include_directories(${CMAKE_SOURCE_DIR}/CompilerDispatcher)
include_directories(${CMAKE_SOURCE_DIR}/Util)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty)

# 源文件
file(GLOB_RECURSE CHTL_SOURCES
    "CHTL/*.cpp"
    "CHTL/*.h"
)

file(GLOB_RECURSE CHTL_JS_SOURCES
    "CHTL_JS/*.cpp"
    "CHTL_JS/*.h"
)

file(GLOB_RECURSE SCANNER_SOURCES
    "Scanner/*.cpp"
    "Scanner/*.h"
)

file(GLOB_RECURSE CODEMERGER_SOURCES
    "CodeMerger/*.cpp"
    "CodeMerger/*.h"
)

file(GLOB_RECURSE DISPATCHER_SOURCES
    "CompilerDispatcher/*.cpp"
    "CompilerDispatcher/*.h"
)

file(GLOB_RECURSE UTIL_SOURCES
    "Util/*.cpp"
    "Util/*.h"
)

# 创建CHTL编译器可执行文件
add_executable(chtl
    ${CHTL_SOURCES}
    ${CHTL_JS_SOURCES}
    ${SCANNER_SOURCES}
    ${CODEMERGER_SOURCES}
    ${DISPATCHER_SOURCES}
    ${UTIL_SOURCES}
    main.cpp
)

# 链接库
target_link_libraries(chtl)

# 测试
enable_testing()
add_subdirectory(Test)