cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

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
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# 查找依赖
find_package(Boost REQUIRED COMPONENTS filesystem system)

# 添加子目录
add_subdirectory(src/CHTL)
add_subdirectory(src/CHTL_JS)
add_subdirectory(src/Scanner)
add_subdirectory(src/CJMOD)
add_subdirectory(src/CodeMerger)
add_subdirectory(src/CompilerDispatcher)
add_subdirectory(src/CSS)
add_subdirectory(src/JS)
add_subdirectory(src/ThirdParty)
add_subdirectory(src/Util)
add_subdirectory(src/Test)
add_subdirectory(src/Module)
add_subdirectory(src/CLI)

# 创建主可执行文件
add_executable(chtl src/main.cpp)
target_link_libraries(chtl 
    CHTLCore
    CHTLJSCore
    Scanner
    CJMOD
    CodeMerger
    CompilerDispatcher
    CSS
    JS
    ThirdParty
    Util
    Module
    ${Boost_LIBRARIES}
)

# 安装规则
install(TARGETS chtl DESTINATION bin)