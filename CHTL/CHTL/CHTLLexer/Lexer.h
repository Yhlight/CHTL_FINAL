#pragma once

#include "Token.h"
#include <string>

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();

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

    void skipWhitespaceAndComments();
    Token makeToken(TokenType type);
    Token errorToken(const std::string& message);
    Token identifier();
    Token stringLiteral();
    Token unquotedLiteral();
    Token comment(TokenType type);

    static bool isAlpha(char c);
    static bool isDigit(char c);
    static bool isIdentifierChar(char c);
};
