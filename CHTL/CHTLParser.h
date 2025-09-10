#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace CHTL {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    NodeList parse();

private:
    // Grammar rule methods
    NodePtr declaration();
    NodePtr elementDeclaration();
    NodePtr textDeclaration();

    // Helper methods for token stream management
    bool isAtEnd();
    const Token& peek();
    const Token& previous();
    const Token& advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    const Token& consume(TokenType type, const std::string& message);

    std::vector<Token> tokens_;
    size_t current_ = 0;
};

// Custom exception for parsing errors
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};

} // namespace CHTL

#endif // CHTL_PARSER_H
