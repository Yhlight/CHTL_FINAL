#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE,     // { }
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]
    LEFT_PAREN, RIGHT_PAREN,     // ( )
    AT,                          // @
    COLON, SEMICOLON,            // : ;
    COMMA,                       // ,
    DOT,                         // .
    QUESTION,                    // ?
    PLUS, MINUS, STAR, SLASH, PERCENT,
    GREATER, LESS,

    // One or two character tokens
    BANG, BANG_EQUAL,            // ! !=
    EQUAL, EQUAL_EQUAL,          // = ==
    GREATER_EQUAL, LESS_EQUAL,    // >= <=
    AMPERSAND, AMPERSAND_AMPERSAND, // & &&
    PIPE, PIPE_PIPE,             // | ||

    // Two-character tokens
    STAR_STAR,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords
    STYLE,
    TEXT,
    INHERIT,

    // Special
    SYMBOL, // For single characters like -, #, etc.
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int position; // Starting character position of the lexeme in the source string
};

// Function to convert TokenType to string for debugging
std::string tokenTypeToString(TokenType type);

} // namespace CHTL

#endif // TOKEN_H
