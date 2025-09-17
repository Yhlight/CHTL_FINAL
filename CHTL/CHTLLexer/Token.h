#pragma once

#include <string>
#include <vector>

enum class TokenType {
    ILLEGAL,
    END_OF_FILE,
    IDENTIFIER,
    STRING_LITERAL,
    UNQUOTED_LITERAL,
    LEFT_BRACE,
    RIGHT_BRACE,
    COLON,
    SEMICOLON,
    EQUALS,
    AMPERSAND,
    DOT,
    POUND,
    AT_SIGN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_PAREN,
    RIGHT_PAREN,
    COMMA,
    HASH_COMMENT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_AT,
    KEYWORD_TOP,
    KEYWORD_BOTTOM,
};

struct Token {
    TokenType type;
    std::string literal;
    int line;

    std::string toString() const;
};

std::string tokenTypeToString(TokenType type);
