# CHTL JS 核心库
add_library(CHTLJSCore STATIC
    CHTLJSCore.cpp
)

# 添加子目录
add_subdirectory(CHTLJSLexer)
add_subdirectory(CHTLJSNode)
add_subdirectory(CHTLJSParser)

# 链接依赖
target_link_libraries(CHTLJSCore PUBLIC
    CHTLJSLexer
    CHTLJSNode
    CHTLJSParser
)

# 设置编译选项
target_compile_features(CHTLJSCore PUBLIC cxx_std_17)