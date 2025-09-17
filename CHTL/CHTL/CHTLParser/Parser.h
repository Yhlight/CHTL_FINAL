#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(Lexer& lexer);
    std::shared_ptr<RootNode> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    // Token manipulation and checking
    Token advance();
    Token peek();
    Token previous();
    bool isAtEnd();
    bool check(TokenType type);
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);

    // Error handling
    void reportError(const std::string& message);
    void synchronize();

    // Parsing grammar rules
    std::shared_ptr<Node> parseDeclaration();
    std::shared_ptr<ElementNode> parseElement();
    std::shared_ptr<AttributeNode> parseAttribute();
    std::shared_ptr<TextNode> parseText();
};
