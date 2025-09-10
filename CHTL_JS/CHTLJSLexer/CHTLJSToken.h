#pragma once

#include <string>
#include <vector>

enum class CHTLJSTokenType {
    // Special
    TOKEN_EOF,
    TOKEN_UNKNOWN,

    // Literals
    TOKEN_IDENTIFIER, // keywords (listen, animate), selectors (.box), values (ease-in-out)
    TOKEN_STRING,
    TOKEN_NUMBER,

    // Symbols
    TOKEN_LBRACE,      // {
    TOKEN_RBRACE,      // }
    TOKEN_COLON,       // :
    TOKEN_COMMA,       // ,
    TOKEN_ARROW,       // ->
    TOKEN_DOUBLE_LBRACE, // {{
    TOKEN_DOUBLE_RBRACE, // }}
    TOKEN_LPAREN,      // (
    TOKEN_RPAREN,      // )
    TOKEN_LBRACKET,    // [
    TOKEN_RBRACKET,    // ]
};

struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string lexeme;
    int line;
};
