cmake_minimum_required(VERSION 3.17)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
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
include_directories(${CMAKE_SOURCE_DIR})

# 查找依赖（暂时注释掉，专注于核心功能）
# find_package(Boost REQUIRED COMPONENTS filesystem system)
# find_package(ANTLR4 REQUIRED)

# 核心库
add_subdirectory(CHTL)
# add_subdirectory(CHTL_JS)
# add_subdirectory(Scanner)
# add_subdirectory(CodeMerger)
# add_subdirectory(CompilerDispatcher)
# add_subdirectory(Util)

# 主程序
add_executable(chtl
    src/main.cpp
)

target_link_libraries(chtl
    CHTLCore
    # CHTLJSCore
    # Scanner
    # CodeMerger
    # CompilerDispatcher
    # Util
    # ${Boost_LIBRARIES}
    # ${ANTLR4_LIBRARIES}
)

# 测试（暂时禁用）
# enable_testing()
# add_subdirectory(Test)

# 安装
install(TARGETS chtl DESTINATION bin)