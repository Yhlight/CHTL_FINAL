#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include <vector>
#include <memory>

class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    std::unique_ptr<ElementNode> parse();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    // Token stream management
    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    const Token& consume(TokenType type, const std::string& message);

    // Parsing rules
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseText();
    void parseBlock(ElementNode* element);
    void parseAttribute(ElementNode* element);
};

#endif // CHTL_PARSER_H
