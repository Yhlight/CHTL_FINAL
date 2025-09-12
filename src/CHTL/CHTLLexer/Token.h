#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Special Tokens
    UNKNOWN,
    END_OF_FILE,

    // Single-character tokens
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    COLON,          // :
    SEMICOLON,      // ;
    EQUAL,          // =

    // Literals
    IDENTIFIER,     // e.g., div, id, my_class
    STRING,         // e.g., "hello world"

    // Keywords (will add more later)
    KEYWORD_TEXT
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line = 0;

    // Helper function for debugging
    static std::string typeToString(TokenType type);
};

} // namespace CHTL
