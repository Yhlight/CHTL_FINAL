#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/Node.h"
#include <vector>
#include <memory>

class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    std::unique_ptr<ProgramNode> parse();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    // Parsing methods
    std::unique_ptr<Node> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseText();
    std::unique_ptr<StyleNode> parseStyle();
    std::unique_ptr<AttributeNode> parseAttribute();
    std::unique_ptr<ValueNode> parseValue();

    // Token helpers
    const Token& peek();
    const Token& previous();
    const Token& advance();
    bool isAtEnd();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    const Token& consume(TokenType type, const std::string& message);

    // Error handling
    void synchronize();
    // In a real parser, we'd have a more robust error reporting system.
};
