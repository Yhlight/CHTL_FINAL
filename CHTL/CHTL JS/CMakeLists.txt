# CHTL JS 编译器库
add_library(CHTLJSCore STATIC
    # 词法分析器
    CHTLJSLexer/CHTLJSToken.cpp
    CHTLJSLexer/CHTLJSLexer.cpp
    
    # 语法分析器
    CHTLJSParser/CHTLJSParser.cpp
    
    # 代码生成器
    CHTLJSGenerator/CHTLJSGenerator.cpp
)

# 设置包含目录
target_include_directories(CHTLJSCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLJSLexer
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLJSParser
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLJSGenerator
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTL/CHTLNode
)

# 链接依赖
target_link_libraries(CHTLJSCore
    CHTLCore
)