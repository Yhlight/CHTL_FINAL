#pragma once

#include "Token.h"
#include <string>

class Lexer {
public:
    Lexer(const std::string& source);
    Token getNextToken();

private:
    void skipWhitespace();
    void skipComment();
    Token readString(char quote);
    Token readIdentifier();
    Token readUnquotedLiteral();
    char peek();
    char advance();

    std::string source;
    size_t position;
    int line;
    int column;
};
