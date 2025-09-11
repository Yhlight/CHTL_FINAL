#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    // Helper methods for scanning
    bool isAtEnd();
    Token scanToken();
    char advance();
    bool match(char expected);
    char peek();
    char peekNext();
    void skipWhitespaceAndComments();

    // Methods for specific token types
    Token makeToken(TokenType type);
    Token errorToken(const std::string& message);
    Token stringLiteral();
    Token identifier();
    Token generatorComment();

    const std::string& source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
};

#endif // CHTL_LEXER_H
