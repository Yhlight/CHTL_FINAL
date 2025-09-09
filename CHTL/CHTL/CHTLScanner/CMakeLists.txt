# CHTL 统一扫描器库
add_library(CHTLScanner STATIC
    UnifiedScanner.cpp
)

target_include_directories(CHTLScanner PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CHTLScanner PUBLIC cxx_std_17)