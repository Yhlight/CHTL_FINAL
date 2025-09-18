#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE,     // { }
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]
    AT,                          // @
    COLON, SEMICOLON,            // : ;

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords
    STYLE,
    TEXT,

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
