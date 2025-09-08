#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_PAREN, RIGHT_PAREN,
    COLON, EQUAL, SEMICOLON,
    COMMA,

    // Literals.
    IDENTIFIER, STRING,

    // Keywords
    TEXT,
    STYLE,
    KEYWORD_TEMPLATE,
    KEYWORD_CUSTOM,
    AT_STYLE,
    AT_ELEMENT,
    AT_VAR,

    // Comments
    LINE_COMMENT,
    BLOCK_COMMENT,
    GENERATOR_COMMENT,

    // End of file.
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};
