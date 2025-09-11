#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

// Enum for all possible token types in CHTL
enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE, // { }
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]
    COLON, SEMICOLON, AT, // : ; @

    // One or two character tokens
    EQUAL, // =
    SLASH, // /
    STAR, // *
    PLUS, // +
    MINUS, // -
    PERCENT, // %
    GREATER, // >
    LESS, // <
    AMPERSAND, // &
    DOT, // .
    HASH, // #
    QUESTION, // ?

    // Multi-character tokens
    STAR_STAR, // **

    // Comment markers
    LINE_COMMENT, // //
    BLOCK_COMMENT_START, // /*
    BLOCK_COMMENT_END, // */
    GEN_COMMENT, // --

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,
    UNQUOTED_LITERAL,

    // Keywords (will be expanded later)
    TEXT,
    STYLE,
    SCRIPT,
    KEYWORD_TEMPLATE,
    KEYWORD_CUSTOM,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_FROM,
    KEYWORD_AS,

    // Special
    UNKNOWN,
    END_OF_FILE
};

// A struct to represent a token
struct Token {
    TokenType type;
    std::string lexeme;
    int line; // Line number for error reporting

    // Helper to convert token type to string for debugging
    std::string toString() const;
};

#endif // CHTL_TOKEN_H
