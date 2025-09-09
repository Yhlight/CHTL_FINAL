add_library(CHTL STATIC
    CHTLLexer/CHTLLexer.cpp
    CHTLLexer/Token.cpp
    CHTLNode/ElementNode.cpp
    CHTLNode/TextNode.cpp
    CHTLNode/CommentNode.cpp
    CHTLParser/CHTLParser.cpp
    CHTLDebug/ASTPrinter.cpp
    CHTLGenerator/CHTLGenerator.cpp
)

target_include_directories(CHTL PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
