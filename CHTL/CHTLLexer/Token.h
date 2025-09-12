#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

namespace CHTL
{
    enum class TokenType
    {
        // Single-character tokens
        LEFT_BRACE, RIGHT_BRACE,  // { }
        LEFT_PAREN, RIGHT_PAREN,  // ( )
        LEFT_BRACKET, RIGHT_BRACKET, // [ ]
        COLON, SEMICOLON,          // : ;
        COMMA, DOT,                // , .
        EQUAL,                     // =
        PLUS, MINUS, STAR, SLASH,  // + - * /
        PERCENT, POWER,            // % **

        // Literals
        IDENTIFIER, // div, text, my-class
        STRING,     // "hello" or 'hello'
        NUMBER,     // 100, 3.14

        // Keywords (will be added as needed)

        // Comments
        SINGLE_LINE_COMMENT, // //
        MULTI_LINE_COMMENT,  // /* */
        GENERATOR_COMMENT,   // --

        // Special
        UNQUOTED_LITERAL,
        AMPERSAND, // &

        // End of File
        END_OF_FILE,

        // Illegal/Unknown token
        ILLEGAL
    };

    struct Token
    {
        TokenType type;
        std::string literal;
        int line; // For error reporting

        Token(TokenType type, std::string literal, int line)
            : type(type), literal(std::move(literal)), line(line) {}
    };

    // For debugging
    std::string tokenTypeToString(TokenType type);
}

#endif // CHTL_TOKEN_H
