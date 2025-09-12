#ifndef CHTL_VALUE_TOKEN_H
#define CHTL_VALUE_TOKEN_H

#include <string>

enum class ValueTokenType {
    // A literal value, like "100px" or "5.5"
    TOKEN_NUMBER,

    // Operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_POWER,

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
