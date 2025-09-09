# CHTL JS 节点库
add_library(CHTLJSNode STATIC
    CHTLJSBaseNode.cpp
    FileLoaderNode.cpp
    EnhancedSelectorNode.cpp
    ListenNode.cpp
    DelegateNode.cpp
    AnimateNode.cpp
    VirNode.cpp
    RouterNode.cpp
    StatementNode.cpp
)

target_include_directories(CHTLJSNode PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLJSNode PUBLIC
    CHTLJSLexer
)

# 设置编译选项
target_compile_features(CHTLJSNode PUBLIC cxx_std_17)