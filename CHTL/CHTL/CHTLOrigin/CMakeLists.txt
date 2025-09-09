# CHTL 原始嵌入系统库
add_library(CHTLOrigin STATIC
    OriginManager.cpp
)

target_include_directories(CHTLOrigin PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLOrigin PUBLIC
    CHTLNode
)

# 设置编译选项
target_compile_features(CHTLOrigin PUBLIC cxx_std_17)