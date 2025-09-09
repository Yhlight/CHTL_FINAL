cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty)

# 查找依赖
find_package(PkgConfig REQUIRED)

# 添加子目录
add_subdirectory(CHTL)
add_subdirectory("CHTL JS")
add_subdirectory(Scanner)
add_subdirectory(CodeMerger)
add_subdirectory(CompilerDispatcher)
add_subdirectory(Util)
add_subdirectory(Test)
add_subdirectory(CLI)

# 主可执行文件
add_executable(chtl main.cpp)
target_link_libraries(chtl 
    CHTLCore
    CHTLJSCore
    Scanner
    CodeMerger
    CompilerDispatcher
    Util
)

# 测试可执行文件
add_executable(test_basic Test/TestBasic.cpp)
target_link_libraries(test_basic 
    CHTLCore
    CHTLJSCore
    Scanner
    CodeMerger
    CompilerDispatcher
    Util
)

add_executable(test_complete Test/TestComplete.cpp)
target_link_libraries(test_complete 
    CHTLCore
    CHTLJSCore
    Scanner
    CodeMerger
    CompilerDispatcher
    Util
)

add_executable(test_chtljs Test/TestCHTLJS.cpp)
target_link_libraries(test_chtljs 
    CHTLCore
    CHTLJSCore
    Scanner
    CodeMerger
    CompilerDispatcher
    Util
)

add_executable(test_configuration Test/TestConfiguration.cpp)
target_link_libraries(test_configuration 
    CHTLCore
    CHTLJSCore
    Scanner
    CodeMerger
    CompilerDispatcher
    Util
)

add_executable(test_module_system Test/TestModuleSystem.cpp)
target_link_libraries(test_module_system 
    CHTLCore
    CHTLJSCore
    Scanner
    CodeMerger
    CompilerDispatcher
    Util
)

add_executable(test_cli Test/TestCLI.cpp)
target_link_libraries(test_cli 
    CHTLCore
    CHTLJSCore
    Scanner
    CodeMerger
    CompilerDispatcher
    Util
)

# 安装规则
install(TARGETS chtl DESTINATION bin)