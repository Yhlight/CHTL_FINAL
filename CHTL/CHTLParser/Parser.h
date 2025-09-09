#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/AST.h"
#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    DocumentNode parse();

private:
    // Parsing methods
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseText();
    AttributeNode parseAttribute();

    // Token management helpers
    Token advance();
    Token peek() const;
    Token peekNext() const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& message);
    bool isAtEnd() const;

    const std::vector<Token>& m_tokens;
    size_t m_current = 0;
};

} // namespace CHTL
