#pragma once

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();
    const std::string& getSource() const { return source; }

private:
    std::string source;
    size_t start;
    size_t current;
    int line;

    bool isAtEnd();
    char advance();
    char peek();
    char peekNext();
    bool match(char expected);
    void skipWhitespace();

    Token makeToken(TokenType type);
    Token errorToken(const std::string& message);
    Token stringToken();
    Token numberToken();
    Token identifierToken();
    Token commentToken();
};
