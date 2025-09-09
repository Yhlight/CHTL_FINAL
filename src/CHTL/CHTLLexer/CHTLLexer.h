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

    Token makeToken(TokenType type, const std::string& lexeme, size_t startOffset);
    Token makeToken(TokenType type, size_t startOffset);
    Token errorToken(const std::string& message);
    Token identifier(size_t startOffset);
    Token unquotedLiteral(size_t startOffset);
    Token stringLiteral(size_t startOffset);

    TokenType identifierType(const std::string& identifier);
};

#endif // CHTL_LEXER_H
