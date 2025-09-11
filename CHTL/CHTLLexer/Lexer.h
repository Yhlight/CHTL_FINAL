#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include "Context/ConfigurationContext.h"

class Lexer {
public:
    Lexer(const std::string& source, const ConfigurationContext& config);
    std::vector<Token> tokenize();

private:
    void buildKeywordMap();
    Token nextToken();
    char peek();
    char advance();
    void skipWhitespace();
    void skipLine();

    Token makeToken(TokenType type) const;
    Token makeToken(TokenType type, const std::string& value) const;
    Token errorToken(const std::string& message) const;

    Token stringLiteral(char quote_type);
    Token identifier();
    Token comment();
    Token operatorToken();

    bool isAtEnd() const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isIdentifierChar(char c) const;

    std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
    int column = 1;

    const ConfigurationContext& config;
    std::map<std::string, TokenType> keyword_map;
};
