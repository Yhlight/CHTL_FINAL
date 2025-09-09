# CHTL 词法分析器库
add_library(CHTLLexer STATIC
    Token.cpp
    Lexer.cpp
    GlobalMap.cpp
)

target_include_directories(CHTLLexer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CHTLLexer PUBLIC cxx_std_17)