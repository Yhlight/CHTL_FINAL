# CHTLCompiler CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# 创建 CHTLCompiler 库
add_library(CHTLCompiler STATIC
    CSSCompiler.cpp
    JSCompiler.cpp
    CompilerManager.cpp
)

# 设置包含目录
target_include_directories(CHTLCompiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 查找必要的库
find_package(Threads REQUIRED)

# 查找可选的库
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(LIBCSS QUIET libcss)
endif()

find_package(v8 QUIET)

# 链接必要的库
target_link_libraries(CHTLCompiler PUBLIC
    Threads::Threads
)

# 链接可选的库
if(TARGET v8::v8)
    target_link_libraries(CHTLCompiler PUBLIC v8::v8 v8::v8_libplatform)
    target_compile_definitions(CHTLCompiler PUBLIC HAVE_V8)
endif()

if(LIBCSS_FOUND)
    target_link_libraries(CHTLCompiler PUBLIC ${LIBCSS_LIBRARIES})
    target_include_directories(CHTLCompiler PUBLIC ${LIBCSS_INCLUDE_DIRS})
    target_compile_definitions(CHTLCompiler PUBLIC HAVE_LIBCSS)
endif()

# 设置 C++ 标准
target_compile_features(CHTLCompiler PUBLIC cxx_std_17)

# 设置编译选项
target_compile_options(CHTLCompiler PRIVATE
    -Wall
    -Wextra
    -O2
)

# 安装规则
install(TARGETS CHTLCompiler
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES
    CSSCompiler.hpp
    JSCompiler.hpp
    CompilerManager.hpp
    DESTINATION include/CHTL/CHTLCompiler
)