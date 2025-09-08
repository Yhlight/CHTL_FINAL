#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACKET, RIGHT_BRACKET,
    COLON, EQUAL, SEMICOLON,
    DOT, HASH, AMPERSAND, COMMA,
    QUESTION, SLASH, STAR, PLUS, MINUS,

    // Two-character tokens
    EQUAL_EQUAL, NOT_EQUAL,
    GREATER_EQUAL, LESS_EQUAL,
    AND_AND, OR_OR,
    GREATER, LESS,


    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    TEXT,
    STYLE,
    KEYWORD_TEMPLATE,
    KEYWORD_CUSTOM,
    KEYWORD_ORIGIN,
    KEYWORD_IMPORT,
    KEYWORD_NAMESPACE,
    KEYWORD_CONFIGURATION,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_EXCEPT,
    KEYWORD_SCRIPT,
    KEYWORD_INHERIT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_ATTOP,
    KEYWORD_ATBOTTOM,
    AT,

    // Comments
    LINE_COMMENT,
    BLOCK_COMMENT,
    GENERATOR_COMMENT,

    // End of file
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    size_t position;
};

TokenType stringToTokenType(const std::string& s);
