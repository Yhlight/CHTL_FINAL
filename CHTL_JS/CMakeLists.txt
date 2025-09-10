add_library(chtl_js_lexer STATIC
    CHTLJSLexer/Lexer.cpp
)

target_include_directories(chtl_js_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
