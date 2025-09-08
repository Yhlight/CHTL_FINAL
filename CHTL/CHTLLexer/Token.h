#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACKET, RIGHT_BRACKET,
    COLON, EQUAL, SEMICOLON,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    TEXT,
    STYLE,
    KEYWORD_TEMPLATE,
    KEYWORD_CUSTOM,
    KEYWORD_INHERIT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_ATTOP,
    KEYWORD_ATBOTTOM,
    AT_STYLE,
    AT_ELEMENT,
    AT_VAR,

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
};
