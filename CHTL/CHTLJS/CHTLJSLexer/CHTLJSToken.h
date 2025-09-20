#ifndef CHTL_JS_TOKEN_H
#define CHTL_JS_TOKEN_H

#include <string>

namespace CHTLJS {

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, COLON,

    // Two-character tokens
    LBRACE_BRACE, RBRACE_BRACE, ARROW,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords (to be added as features are implemented)
    LISTEN, ANIMATE, DELEGATE, VIR,

    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

} // namespace CHTLJS

#endif // CHTL_JS_TOKEN_H
