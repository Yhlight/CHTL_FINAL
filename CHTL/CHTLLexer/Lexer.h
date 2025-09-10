#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <map>

class Lexer {
public:
    // Constructor
    Lexer(const std::string& input);

    // Main method to generate all tokens from the input
    std::vector<Token> tokenize();

private:
    // Input source code
    const std::string& m_input;
    // Current position in input (points to current char)
    size_t m_position;
    // Current reading position in input (after current char)
    size_t m_readPosition;
    // Current char under examination
    char m_ch;
    // Current line number for error reporting
    int m_line;

    // Map to store keywords
    static const std::map<std::string, TokenType> s_keywords;

    // Helper methods
    void readChar();
    char peekChar() const;
    Token nextToken();
    void skipWhitespace();

    // Helpers for reading specific token types
    Token readIdentifier();
    Token readStringLiteral();
    Token readNumberLiteral();
    Token readComment();

    // Helper to check for character types
    bool isLetter(char ch) const;
    bool isDigit(char ch) const;

    // Helper to create a new token
    Token newToken(TokenType type, const std::string& lexeme) const;
};

#endif //CHTL_LEXER_H
