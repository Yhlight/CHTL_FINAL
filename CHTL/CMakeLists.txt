add_library(chtl_lib STATIC
    CHTLLexer/CHTLLexer.cpp
    CHTLParser/CHTLParser.cpp
    CHTLGenerator/CHTLGenerator.cpp
)

target_include_directories(chtl_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
