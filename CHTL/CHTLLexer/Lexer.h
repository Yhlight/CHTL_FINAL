#pragma once

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    Token getNextToken();
    char peek();
    char peekNext();
    void enterRawMode();
    void exitRawMode();

private:
    bool rawMode = false;
    std::string source;
    size_t current = 0;
    int line = 1;
    int column = 1;

    char advance();
    bool isAtEnd();
    Token makeToken(TokenType type, const std::string& lexeme);
    Token errorToken(const std::string& message);

    void skipWhitespaceAndComments();
    Token identifier();
    Token stringLiteral();
    Token number();
};
