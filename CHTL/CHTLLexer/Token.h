#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

enum class TokenType {
    // Keywords recognized by the parser
    TOKEN_TEXT,
    TOKEN_STYLE,
    TOKEN_ELEMENT,

    // Any other word-like sequence. The parser will determine if it's a
    // tag name, attribute name, or unquoted literal value.
    TOKEN_IDENTIFIER,

    // Symbols
    TOKEN_LBRACE,   // {
    TOKEN_RBRACE,   // }
    TOKEN_LBRACKET, // [
    TOKEN_RBRACKET, // ]
    TOKEN_AT,       // @
    TOKEN_COLON,    // :
    TOKEN_EQUAL,    // =
    TOKEN_SEMICOLON, // ;

    // Literals
    TOKEN_STRING_LITERAL, // "hello" or 'hello'

    // Comments
    TOKEN_GENERATOR_COMMENT, // -- a comment that gets generated

    // Misc
    TOKEN_EOF,      // End of File
    TOKEN_ERROR     // Represents a lexical error
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    size_t offset; // The starting character offset in the source string
};

// Helper function for debugging
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::TOKEN_TEXT: return "TEXT";
        case TokenType::TOKEN_STYLE: return "STYLE";
        case TokenType::TOKEN_ELEMENT: return "ELEMENT";
        case TokenType::TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TokenType::TOKEN_LBRACE: return "LBRACE";
        case TokenType::TOKEN_RBRACE: return "RBRACE";
        case TokenType::TOKEN_LBRACKET: return "LBRACKET";
        case TokenType::TOKEN_RBRACKET: return "RBRACKET";
        case TokenType::TOKEN_AT: return "AT";
        case TokenType::TOKEN_COLON: return "COLON";
        case TokenType::TOKEN_EQUAL: return "EQUAL";
        case TokenType::TOKEN_SEMICOLON: return "SEMICOLON";
        case TokenType::TOKEN_STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::TOKEN_GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::TOKEN_EOF: return "EOF";
        case TokenType::TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}


#endif // CHTL_TOKEN_H
