#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    void addToken(TokenType type);
    bool match(char expected);
    char peek();
    char peekNext();
    void string(char quote);
    void identifier();

    const std::string m_source;
    std::vector<Token> m_tokens;
    int m_start = 0;
    int m_current = 0;
    int m_line = 1;
};

} // namespace CHTL
