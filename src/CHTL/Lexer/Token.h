#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <variant>

namespace CHTL {

enum class TokenType {
    // Keywords
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    // ... other keywords will be added later

    // Literals
    IDENTIFIER,
    STRING_LITERAL,
    UNQUOTED_LITERAL,

    // Symbols
    LEFT_BRACE,   // {
    RIGHT_BRACE,  // }
    COLON,        // :
    SEMICOLON,    // ;
    EQUALS,       // =

    // Comments
    COMMENT,

    // Misc
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

} // namespace CHTL

#endif // CHTL_TOKEN_H
