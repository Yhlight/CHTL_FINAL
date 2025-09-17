#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<RootNode> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    Token advance();
    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);

    std::shared_ptr<BaseNode> parseStatement();
    std::shared_ptr<ElementNode> parseElement();
    void parseBlock(std::shared_ptr<ElementNode> element);
    void parseAttribute(std::shared_ptr<ElementNode> element);
    std::shared_ptr<TextNode> parseTextObject();
};
