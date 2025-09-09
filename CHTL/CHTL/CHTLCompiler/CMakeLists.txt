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

# 链接必要的库
target_link_libraries(CHTLCompiler PUBLIC
    # 这里可以链接外部 CSS/JS 编译器库
    # 例如：libcss, v8, quickjs 等
)

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