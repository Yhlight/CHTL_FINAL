#pragma once

#include "Token.h"
#include <string>
#include <vector>

class CHTLLexer {
public:
    CHTLLexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t current = 0;
    size_t start = 0;
    int line = 1;
    int column = 1;

    Token nextToken();
    Token makeToken(TokenType type);
    Token makeToken(TokenType type, const std::string& lexeme);
    Token errorToken(const std::string& message);

    char advance();
    bool match(char expected);
    char peek();
    char peekNext();
    bool isAtEnd();

    void skipWhitespaceAndComments();
    Token stringLiteral();
    Token identifier();
    Token number(); // For simplicity, we'll start by treating numbers as part of unquoted literals or identifiers

    TokenType checkKeyword(int start, int length, const char* rest);
    TokenType identifierType();
};
