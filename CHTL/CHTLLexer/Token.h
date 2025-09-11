#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COLON, SEMICOLON, AT,

    // Operators
    EQUAL, PLUS, MINUS, STAR, SLASH, PERCENT,
    GREATER, LESS, AMPERSAND, DOT, HASH, QUESTION,

    // Multi-character tokens
    STAR_STAR,
    GEN_COMMENT, // --

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords
    TEXT,
    STYLE,
    SCRIPT,
    KEYWORD_ORIGIN,
    KEYWORD_IMPORT,
    KEYWORD_FROM,
    KEYWORD_AS,

    // Special
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::string content; // Used for [Origin] blocks
    int line;

    std::string toString() const;
};

#endif // CHTL_TOKEN_H
