add_library(chtl_js_lexer STATIC
    CHTLJSLexer/Lexer.cpp
)
target_include_directories(chtl_js_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_library(chtl_js_parser STATIC
    CHTLJSParser/Parser.cpp
)
target_include_directories(chtl_js_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(chtl_js_parser PRIVATE chtl_js_lexer)

add_library(chtl_js_generator STATIC
    CHTLJSGenerator/Generator.cpp
)
target_include_directories(chtl_js_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
