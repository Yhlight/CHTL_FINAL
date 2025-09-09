# CHTL 管理器库
add_library(CHTLManage STATIC
    TemplateManager.cpp
)

target_include_directories(CHTLManage PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLManage PUBLIC
    CHTLNode
)

# 设置编译选项
target_compile_features(CHTLManage PUBLIC cxx_std_17)