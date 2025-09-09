# CHTL 约束系统库
add_library(CHTLConstraint STATIC
    ConstraintManager.cpp
)

target_include_directories(CHTLConstraint PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLConstraint PUBLIC
    CHTLNode
)

# 设置编译选项
target_compile_features(CHTLConstraint PUBLIC cxx_std_17)