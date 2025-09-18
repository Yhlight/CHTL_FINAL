add_library(CHTL STATIC
    CHTLGenerator/CHTLGenerator.cpp
    CHTLLexer/CHTLLexer.cpp
    CHTLLexer/Token.cpp
    CHTLLoader/CHTLLoader.cpp
    CHTLNode/ElementNode.cpp
    CHTLNode/OriginNode.cpp
    CHTLNode/StyleNode.cpp
    CHTLNode/TextNode.cpp
    CHTLNode/DocumentNode.cpp
    CHTLParser/CHTLParser.cpp
    Expression/ExpressionEvaluator.cpp
)

target_include_directories(CHTL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(CHTL PRIVATE Util)
