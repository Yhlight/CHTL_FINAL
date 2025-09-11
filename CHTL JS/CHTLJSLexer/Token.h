#pragma once

#include <string>
#include <vector>

namespace CHTL_JS {

enum class TokenType {
    TOKEN_ILLEGAL,
    TOKEN_EOF,

    // Delimiters for enhanced selectors
    TOKEN_LDBRACE, // {{
    TOKEN_RDBRACE, // }}

    // Content inside selectors
    TOKEN_IDENTIFIER, // selector parts like 'div', 'my-class'
    TOKEN_DOT,        // .
    TOKEN_HASH,       // #
    TOKEN_LBRACKET,   // [
    TOKEN_RBRACKET,   // ]
    TOKEN_NUMBER,     // 0

    // For now, other JS code will be treated as raw strings/identifiers
    TOKEN_JS_CODE,
};

struct Token {
    TokenType type;
    std::string literal;
};

} // namespace CHTL_JS
