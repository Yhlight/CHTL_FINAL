# CHTL 模块系统库
add_library(CHTLModule STATIC
    ModuleManager.cpp
    CJMODAPI.cpp
)

target_include_directories(CHTLModule PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLModule PUBLIC
    CHTLNode
    CHTLManage
    CHTLJSCore
)

# 设置编译选项
target_compile_features(CHTLModule PUBLIC cxx_std_17)