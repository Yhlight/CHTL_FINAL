#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Special Tokens
    TOKEN_ILLEGAL, // Token/character we don't know about
    TOKEN_EOF,     // End Of File

    // Identifiers + literals
    TOKEN_IDENTIFIER, // add, foobar, x, y, ...
    TOKEN_STRING,     // "foobar" or 'foobar' or foobar

    // Operators & Delimiters
    TOKEN_ASSIGN,     // =
    TOKEN_COLON,      // :
    TOKEN_SEMICOLON,  // ;
    TOKEN_LPAREN,     // (
    TOKEN_RPAREN,     // )
    TOKEN_LBRACE,     // {
    TOKEN_RBRACE,     // }
    TOKEN_LBRACKET,   // [
    TOKEN_RBRACKET,   // ]
    TOKEN_COMMA,      // ,
    TOKEN_DOT,        // .
    TOKEN_HASH,       // #
    TOKEN_AMPERSAND,  // &

    // Arithmetic & Logical Operators
    TOKEN_PLUS,       // +
    TOKEN_MINUS,      // -
    TOKEN_STAR,       // *
    TOKEN_SLASH,      // /
    TOKEN_PERCENT,    // %
    TOKEN_POWER,      // **
    TOKEN_GT,         // >
    TOKEN_LT,         // <
    TOKEN_QUESTION,   // ?
    TOKEN_AND,        // &&
    TOKEN_OR,         // ||
    TOKEN_AT,         // @

    // Keywords
    TOKEN_KEYWORD_TEMPLATE, // [Template]
    TOKEN_KEYWORD_CUSTOM,   // [Custom]
    TOKEN_KEYWORD_IMPORT,   // [Import]
    TOKEN_KEYWORD_DELETE,
    TOKEN_KEYWORD_INSERT,
    TOKEN_KEYWORD_AFTER,
    TOKEN_KEYWORD_BEFORE,
    TOKEN_KEYWORD_REPLACE,
    TOKEN_KEYWORD_ATTOP,
    TOKEN_KEYWORD_ATBOTTOM,
    TOKEN_KEYWORD_FROM,
    TOKEN_KEYWORD_AS,

    // Comments (might be filtered out by the lexer directly)
    TOKEN_COMMENT,    // // or /* */
};

struct Token {
    TokenType type;
    std::string literal;
    int line = 0;
    int column = 0;

    std::string ToString() const {
        return "Line " + std::to_string(line) + ":" + std::to_string(column) +
               " | Type: " + std::to_string(static_cast<int>(type)) +
               " | Literal: " + literal;
    }
};

} // namespace CHTL
