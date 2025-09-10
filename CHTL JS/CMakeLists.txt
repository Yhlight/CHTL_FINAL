add_library(chtl_js_lib STATIC
    CHTLJSLexer/CHTLJSLexer.cpp
    CHTLJSParser/CHTLJSParser.cpp
    CHTLJSGenerator/CHTLJSGenerator.cpp
)

target_include_directories(chtl_js_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
