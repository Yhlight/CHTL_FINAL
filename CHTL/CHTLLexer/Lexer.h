#pragma once

#include "Token.h"
#include <string>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& source);
    Token getNextToken();
    std::vector<Token> getAllTokens();

private:
    std::string m_source;
    size_t m_position;
    int m_line;
    int m_column;

    char peek(size_t offset = 0) const;
    char consume();

    Token makeToken(TokenType type, const std::string& value);
    Token makeToken(TokenType type, char value);

    void skipWhitespace();
    Token lexIdentifierOrUnquotedLiteral();
    Token lexStringLiteral();
    Token lexComment();
};

} // namespace CHTL
