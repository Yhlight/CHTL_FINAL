#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE,
    COLON, EQUAL, SEMICOLON,

    // Literals.
    IDENTIFIER, STRING,

    // Keywords
    TEXT,
    STYLE,

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
