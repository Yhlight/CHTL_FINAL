# Create a static library for the CHTL compiler components
add_library(CHTLCompiler STATIC
    CHTLLexer/Lexer.cpp
    CHTLNode/ElementNode.cpp
    CHTLNode/AttributeNode.cpp
    CHTLNode/TextNode.cpp
    CHTLNode/CommentNode.cpp
    CHTLNode/StyleNode.cpp
    CHTLNode/TemplateDefinitionNode.cpp
    CHTLParser/Parser.cpp
    AstPrinter.cpp
    CHTLGenerator/HtmlGenerator.cpp
    CSSExt/Expression.cpp
    CSSExt/ExpressionParser.cpp
    CSSExt/ExpressionEvaluator.cpp
)

# Publicly expose the include directory for this library
target_include_directories(CHTLCompiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link against utility library
target_link_libraries(CHTLCompiler PUBLIC Util)
