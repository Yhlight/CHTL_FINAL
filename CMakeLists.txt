cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")

# 查找依赖库
find_package(PkgConfig REQUIRED)

# 查找 ANTLR4 (可选)
find_package(antlr4-runtime QUIET)
if(antlr4-runtime_FOUND)
    set(ANTLR4_AVAILABLE TRUE)
    message(STATUS "ANTLR4 found: ${ANTLR4_LIBRARIES}")
else()
    set(ANTLR4_AVAILABLE FALSE)
    message(STATUS "ANTLR4 not found, using simplified implementation")
endif()

# 查找 libsass (可选)
pkg_check_modules(LIBSASS libsass)
if(LIBSASS_FOUND)
    set(LIBSASS_AVAILABLE TRUE)
    message(STATUS "libsass found: ${LIBSASS_LIBRARIES}")
else()
    set(LIBSASS_AVAILABLE FALSE)
    message(STATUS "libsass not found, using simplified implementation")
endif()

# 查找 V8 (可选)
pkg_check_modules(V8 libv8)
if(V8_FOUND)
    set(V8_AVAILABLE TRUE)
    message(STATUS "V8 found: ${V8_LIBRARIES}")
else()
    set(V8_AVAILABLE FALSE)
    message(STATUS "V8 not found, using simplified implementation")
endif()

# 查找 JSON 库 (必需)
find_package(nlohmann_json QUIET)
if(nlohmann_json_FOUND)
    set(JSON_AVAILABLE TRUE)
    message(STATUS "nlohmann_json found")
else()
    set(JSON_AVAILABLE FALSE)
    message(STATUS "nlohmann_json not found, using simplified implementation")
endif()

# 查找文件系统库
find_library(FILESYSTEM_LIB stdc++fs)
if(NOT FILESYSTEM_LIB)
    set(FILESYSTEM_LIB "")
endif()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

if(ANTLR4_AVAILABLE)
    include_directories(${ANTLR4_INCLUDE_DIRS})
endif()

if(LIBSASS_AVAILABLE)
    include_directories(${LIBSASS_INCLUDE_DIRS})
endif()

# 添加子目录
add_subdirectory(src)

# 创建可执行文件
add_executable(CHTL_Compiler
    src/main.cpp
)

# 链接库
target_link_libraries(CHTL_Compiler
    CHTLCore
    ${FILESYSTEM_LIB}
)

# 条件链接外部库
if(ANTLR4_AVAILABLE)
    target_link_libraries(CHTL_Compiler ${ANTLR4_LIBRARIES})
    target_compile_definitions(CHTL_Compiler PRIVATE ANTLR4_AVAILABLE)
endif()

if(LIBSASS_AVAILABLE)
    target_link_libraries(CHTL_Compiler ${LIBSASS_LIBRARIES})
    target_compile_definitions(CHTL_Compiler PRIVATE LIBSASS_AVAILABLE)
endif()

if(V8_AVAILABLE)
    target_link_libraries(CHTL_Compiler ${V8_LIBRARIES})
    target_compile_definitions(CHTL_Compiler PRIVATE V8_AVAILABLE)
endif()

if(JSON_AVAILABLE)
    target_link_libraries(CHTL_Compiler nlohmann_json::nlohmann_json)
    target_compile_definitions(CHTL_Compiler PRIVATE JSON_AVAILABLE)
endif()

# 设置输出目录
set_target_properties(CHTL_Compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS CHTL_Compiler DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/test DESTINATION share/chtl)

# 测试
enable_testing()
add_subdirectory(test)

# 打包
set(CPACK_PACKAGE_NAME "CHTL_Compiler")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHTL HyperText Language Compiler")
set(CPACK_PACKAGE_VENDOR "CHTL Team")
set(CPACK_PACKAGE_CONTACT "chtl@example.com")

include(CPack)