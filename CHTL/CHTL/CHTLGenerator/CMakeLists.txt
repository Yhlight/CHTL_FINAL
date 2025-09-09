# CHTL 代码生成器库
add_library(CHTLGenerator STATIC
    CodeGenerator.cpp
)

target_include_directories(CHTLGenerator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLGenerator PUBLIC
    CHTLNode
    CHTLManage
)

# 设置编译选项
target_compile_features(CHTLGenerator PUBLIC cxx_std_17)