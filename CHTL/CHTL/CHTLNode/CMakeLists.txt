# CHTL 节点库
add_library(CHTLNode STATIC
    BaseNode.cpp
    ElementNode.cpp
    TextNode.cpp
    StyleNode.cpp
    TemplateNode.cpp
    TemplateStyleNode.cpp
    TemplateElementNode.cpp
    TemplateVarNode.cpp
    CustomNode.cpp
    CustomStyleNode.cpp
    CustomElementNode.cpp
    CustomVarNode.cpp
    OriginNode.cpp
    ImportNode.cpp
    ConfigNode.cpp
    NamespaceNode.cpp
)

target_include_directories(CHTLNode PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CHTLNode PUBLIC cxx_std_17)