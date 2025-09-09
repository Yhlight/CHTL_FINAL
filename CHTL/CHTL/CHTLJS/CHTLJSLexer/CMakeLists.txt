# CHTL JS 词法分析器库
add_library(CHTLJSLexer STATIC
    CHTLJSToken.cpp
    CHTLJSLexer.cpp
)

target_include_directories(CHTLJSLexer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CHTLJSLexer PUBLIC cxx_std_17)