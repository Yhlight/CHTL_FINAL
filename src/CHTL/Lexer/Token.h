#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <variant>

namespace CHTL {

enum class TokenType {
    // Keywords
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_TEMPLATE,
    KEYWORD_CUSTOM,
    KEYWORD_ELEMENT,
    KEYWORD_VAR,
    KEYWORD_DELETE,

    // Literals
    IDENTIFIER,
    STRING_LITERAL,
    UNQUOTED_LITERAL,

    // Symbols
    LEFT_BRACE,   // {
    RIGHT_BRACE,  // }
    LEFT_BRACKET, // [
    RIGHT_BRACKET,// ]
    LEFT_PAREN,   // (
    RIGHT_PAREN,  // )
    AT_SIGN,      // @
    COLON,        // :
    SEMICOLON,    // ;
    EQUALS,       // =
    COMMA,        // ,

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
