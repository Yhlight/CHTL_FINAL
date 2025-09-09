# CHTL 模板系统库
add_library(CHTLTemplate STATIC
    TemplateManager.cpp
)

target_include_directories(CHTLTemplate PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLTemplate PUBLIC
    CHTLNode
)

# 设置编译选项
target_compile_features(CHTLTemplate PUBLIC cxx_std_17)