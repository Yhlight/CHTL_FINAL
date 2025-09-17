#pragma once

#include <string>
#include <vector>

// Enum for all possible token types in CHTL
enum class TokenType {
    // Special Tokens
    TOKEN_EOF,
    TOKEN_ERROR,

    // Literals
    TOKEN_IDENTIFIER,       // e.g., div, id, my_variable
    TOKEN_STRING_LITERAL,   // e.g., "hello world", 'some text'
    TOKEN_UNQUOTED_LITERAL, // e.g., red, 100px, some-value

    // Symbols
    TOKEN_LEFT_BRACE,       // {
    TOKEN_RIGHT_BRACE,      // }
    TOKEN_COLON,            // :
    TOKEN_EQUALS,           // =
    TOKEN_SEMICOLON,        // ;
    TOKEN_LEFT_BRACKET,     // [
    TOKEN_RIGHT_BRACKET,    // ]
    TOKEN_AT,               // @
    TOKEN_DOLLAR,           // $
    TOKEN_AMPERSAND,        // &
    TOKEN_PLUS,             // +
    TOKEN_MINUS,            // -
    TOKEN_STAR,             // *
    TOKEN_SLASH,            // /
    TOKEN_PERCENT,          // %

    // Comments
    TOKEN_COMMENT_NORMAL,      // // or /* ... */
    TOKEN_COMMENT_GENERATOR,   // # a comment

    // Keywords (will be added as needed)
    // For now, we can treat them as identifiers and let the parser decide.
};

// Represents a single token extracted by the Lexer
struct Token {
    TokenType type;
    std::string value;
    int line;
};
