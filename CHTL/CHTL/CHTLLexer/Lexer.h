#pragma once

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t current = 0;
    size_t line = 1;
    size_t column = 1;

    char advance();
    char peek() const;
    char peekNext() const;
    bool isAtEnd() const;

    Token makeToken(TokenType type, const std::string& value = "");
    Token makeToken(TokenType type, char value);

    void skipWhitespaceAndComments();
    Token scanToken();
    Token scanIdentifierOrKeyword();
    Token scanStringLiteral();
    Token scanNumberLiteral();
    Token scanSymbol();
};
