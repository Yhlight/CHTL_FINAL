#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Special Tokens
    TOKEN_EOF,
    TOKEN_UNKNOWN,

    // Literals
    TOKEN_IDENTIFIER, // e.g., div, id, color
    TOKEN_STRING,     // e.g., "Hello World"
    TOKEN_NUMBER,     // e.g., 100, 3.14

    // Symbols
    TOKEN_LBRACE,     // {
    TOKEN_RPAREN,     // )
    TOKEN_LPAREN,     // (
    TOKEN_RBRACE,     // }
    TOKEN_COLON,      // :
    TOKEN_EQUAL,      // =
    TOKEN_SEMICOLON,  // ;
    TOKEN_DOT,        // .
    TOKEN_HASH,       // #
    TOKEN_AMPERSAND,  // &
    TOKEN_AT,         // @

    // Operators
    TOKEN_PLUS,       // +
    TOKEN_MINUS,      // -
    TOKEN_STAR,       // *
    TOKEN_SLASH,      // /
    TOKEN_PERCENT,    // %
    TOKEN_STAR_STAR,  // **

    // Comparison
    TOKEN_GREATER,    // >
    TOKEN_LESS,       // <
    TOKEN_GREATER_EQUAL, // >=
    TOKEN_LESS_EQUAL,    // <=
    TOKEN_EQUAL_EQUAL,   // ==
    TOKEN_BANG_EQUAL,    // !=

    // Logical
    TOKEN_AMPERSAND_AMPERSAND, // &&
    TOKEN_PIPE_PIPE,         // ||
    TOKEN_QUESTION,          // ?


    // Comments
    TOKEN_LINE_COMMENT,      // // ...
    TOKEN_BLOCK_COMMENT,     // /* ... */
    TOKEN_GENERATOR_COMMENT, // -- ...

    // Keywords (will be handled as identifiers initially, but good to have)
    TOKEN_KEYWORD_TEXT,
    TOKEN_KEYWORD_STYLE,
        KEYWORD_ORIGIN,
        KEYWORD_NAMESPACE,
        KEYWORD_IMPORT,
        KEYWORD_FROM,
        KEYWORD_AS,
        KEYWORD_TEMPLATE,
        KEYWORD_CUSTOM,
        KEYWORD_DELETE,
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    size_t start;
};
