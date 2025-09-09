# CHTL 配置系统库
add_library(CHTLConfig STATIC
    ConfigManager.cpp
)

target_include_directories(CHTLConfig PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLConfig PUBLIC
    CHTLNode
)

# 设置编译选项
target_compile_features(CHTLConfig PUBLIC cxx_std_17)