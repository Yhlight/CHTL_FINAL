#ifndef CHTL_JS_TOKEN_H
#define CHTL_JS_TOKEN_H

#include <string>
#include <vector>

namespace CHTL_JS {

enum class TokenType {
    // Delimiters
    LEFT_BRACE, RIGHT_BRACE,     // { }
    LEFT_PAREN, RIGHT_PAREN,     // ( )
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]
    LEFT_BRACE_BRACE, RIGHT_BRACE_BRACE, // {{ }}
    COLON, SEMICOLON,            // : ;
    COMMA,                       // ,
    DOT,                         // .

    // Operators
    ARROW,                      // ->
    AMPERSAND_ARROW,            // &->

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords
    LISTEN,
    DELEGATE,
    ANIMATE,

    // Special
    SYMBOL,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line; // for error reporting
    int position;
};

std::string tokenTypeToString(TokenType type);

} // namespace CHTL_JS

#endif // CHTL_JS_TOKEN_H
