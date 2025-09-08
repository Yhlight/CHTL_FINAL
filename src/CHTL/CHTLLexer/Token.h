#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Special Tokens
    ILLEGAL, // Represents a token that is not valid
    END_OF_FILE,

    // Identifiers & Literals
    IDENTIFIER,   // e.g., div, id, class
    STRING,       // "a string" or 'a string'
    UNQUOTED_LITERAL, // a string without quotes

    // Operators & Delimiters
    L_BRACE,      // {
    R_BRACE,      // }
    L_PAREN,      // (
    R_PAREN,      // )
    L_BRACKET,    // [
    R_BRACKET,    // ]
    COLON,        // :
    SEMICOLON,    // ;
    EQUALS,       // =
    COMMA,        // ,
    AT,           // @
    AMPERSAND,    // &
    HASH,         // #
    DOT,          // .
    GREATER_THAN, // >
    LESS_THAN,    // <
    QUESTION,     // ?
    PLUS,         // +
    MINUS,        // -
    ASTERISK,     // *
    SLASH,        // /

    // Comments
    SINGLE_LINE_COMMENT, // // a comment
    MULTI_LINE_COMMENT,  // /* a comment */
    GENERATOR_COMMENT,   // -- a comment

    // Keywords (will be expanded later)
    KEYWORD_TEXT
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};
