#pragma once

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();

private:
    void readChar();
    char peekChar() const;
    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();

    Token makeToken(TokenType type, const std::string& literal);
    Token readIdentifier();
    Token readStringLiteral();
    Token readHashComment();

    std::string source;
    size_t position;
    size_t readPosition;
    char ch;
    int line;
};
