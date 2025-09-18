#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE, // { }
    COLON, SEMICOLON,       // : ;

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords - will be added later

    // Special
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

// Function to convert TokenType to string for debugging
std::string tokenTypeToString(TokenType type);

} // namespace CHTL

#endif // TOKEN_H
