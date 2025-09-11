#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    const std::string& source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    static const std::unordered_map<std::string, TokenType> keywords;

    bool isAtEnd();
    void scanToken();
    char advance();
    bool match(char expected);
    char peek();
    char peekNext();

    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal, const std::string& content);

    void scanString(char quoteType);
    void scanNumber();
    void scanIdentifier();
};

#endif // CHTL_LEXER_H
