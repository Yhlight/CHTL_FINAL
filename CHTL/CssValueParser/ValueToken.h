#ifndef CHTL_VALUE_TOKEN_H
#define CHTL_VALUE_TOKEN_H

#include <string>

enum class ValueTokenType {
    // Literals
    TOKEN_NUMBER,       // e.g., "100px"
    TOKEN_IDENTIFIER,   // e.g., a selector like 'box' or a property name like 'width'
    TOKEN_STRING,       // e.g., 'red', 'solid'

    // Operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_POWER,
    TOKEN_DOT,      // .

    // Conditional and Logical Operators
    TOKEN_QUESTION, // ?
    TOKEN_COLON,    // :
    TOKEN_GREATER,  // >
    TOKEN_GREATER_EQUAL, // >=
    TOKEN_LESS,     // <
    TOKEN_LESS_EQUAL,    // <=
    TOKEN_EQUAL_EQUAL,   // ==
    TOKEN_NOT_EQUAL,   // !=
    TOKEN_LOGICAL_AND, // &&
    TOKEN_LOGICAL_OR,  // ||

    // Parentheses
    TOKEN_LPAREN,
    TOKEN_RPAREN,

    // Misc
    TOKEN_EOF,
    TOKEN_ERROR
};

struct ValueToken {
    ValueTokenType type;
    std::string text;
};

#endif // CHTL_VALUE_TOKEN_H
