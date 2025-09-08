#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <vector>
#include <memory>

class StyleNode; // Forward declaration

class Parser {
public:
    Parser(Lexer& lexer);
    std::unique_ptr<ElementNode> parse();

private:
    Lexer& lexer;
    Token currentToken;
    Token nextToken;

    void advance();
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);
    bool check(TokenType type);
    bool checkNext(TokenType type);

    std::unique_ptr<BaseNode> declaration();
    std::unique_ptr<ElementNode> element();
    std::unique_ptr<TextNode> textNode();
    std::unique_ptr<StyleNode> styleNode();
    void attributes(ElementNode& element);
};
