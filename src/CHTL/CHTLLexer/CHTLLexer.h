#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

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
    int line = 1;
    int column = 1;

    char peek();
    char advance();
    bool isAtEnd();
    void skipWhitespaceAndComments();

    Token makeToken(TokenType type, const std::string& lexeme);
    Token makeToken(TokenType type);
    Token errorToken(const std::string& message);
    Token identifier();
    Token unquotedLiteral();
    Token stringLiteral();

    TokenType identifierType(const std::string& identifier);
};

#endif // CHTL_LEXER_H
