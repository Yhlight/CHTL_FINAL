# CHTL 命名空间系统库
add_library(CHTLNamespace STATIC
    NamespaceManager.cpp
)

target_include_directories(CHTLNamespace PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLNamespace PUBLIC
    CHTLNode
)

# 设置编译选项
target_compile_features(CHTLNamespace PUBLIC cxx_std_17)