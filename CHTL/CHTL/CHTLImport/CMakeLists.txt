# CHTL 导入系统库
add_library(CHTLImport STATIC
    ImportManager.cpp
)

target_include_directories(CHTLImport PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLImport PUBLIC
    CHTLNode
)

# 设置编译选项
target_compile_features(CHTLImport PUBLIC cxx_std_17)