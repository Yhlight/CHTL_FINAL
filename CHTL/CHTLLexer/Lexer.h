#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <map>

class Lexer {
public:
    Lexer(const std::string& source);

    // Scans all tokens from the source and returns them as a vector.
    std::vector<Token> scanTokens();

private:
    const std::string& source;
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
    int column = 1;

    // A map to quickly look up keywords
    static const std::map<std::string, TokenType> keywords;

    void scanToken();
    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);

    void addToken(TokenType type);

    // Specific token scanning routines
    void stringLiteral();
    void numberLiteral();
    void identifier();
    void blockComment();
    void lineComment();
    void generatorComment();

    // The logic for unquoted literals is highly context-dependent.
    // The lexer will emit IDENTIFIER tokens, and the parser will be responsible
    // for interpreting them as unquoted literals in the appropriate context
    // (e.g., inside a `text{}` block or as a property value).

    // Character classification helpers
    static bool isDigit(char c);
    static bool isAlpha(char c);
    static bool isAlphaNumeric(char c);
};

#endif // LEXER_H
