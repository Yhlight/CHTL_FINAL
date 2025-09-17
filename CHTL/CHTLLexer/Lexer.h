#pragma once

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();
    std::vector<Token> getAllTokens();

private:
    void readChar();
    char peekChar() const;
    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();

    Token makeToken(TokenType type, const std::string& literal);
    Token readIdentifier();
    Token readStringLiteral();
    Token readUnquotedLiteral();
    Token readHashComment();

    std::string source;
    size_t position;      // current position in input (points to current char)
    size_t readPosition;  // current reading position in input (after current char)
    char ch;              // current char under examination
    int line;             // current line number
};
