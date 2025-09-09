# CHTL 自定义系统库
add_library(CHTLCustom STATIC
    CustomManager.cpp
)

target_include_directories(CHTLCustom PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLCustom PUBLIC
    CHTLNode
)

# 设置编译选项
target_compile_features(CHTLCustom PUBLIC cxx_std_17)