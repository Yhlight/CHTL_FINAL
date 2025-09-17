#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Special tokens
    ILLEGAL,
    END_OF_FILE,

    // Identifiers & literals
    IDENTIFIER, // e.g., div, body, my_variable
    STRING,     // "hello world"

    // Delimiters
    AT,         // @
    LBRACE,     // {
    RBRACE,     // }
    LPAREN,     // (
    RPAREN,     // )
    LBRACKET,   // [
    RBRACKET,   // ]
    SEMICOLON,  // ;
    COLON,      // :
    COMMA,      // ,
    DOT,        // .

    // Operators
    ASSIGN,     // =
    PLUS,       // +
    MINUS,      // -
    ASTERISK,   // *
    SLASH,      // /
    PERCENT,    // %
    POWER,      // **

    // Comments (usually skipped, but tokenizing can be useful for tools)
    COMMENT,

    // Keywords will be added later
};

struct Token {
    TokenType type;
    std::string literal;
    int line;
    int column;
};

} // namespace CHTL
