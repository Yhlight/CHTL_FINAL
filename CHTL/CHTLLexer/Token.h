#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    LEFT_PAREN, RIGHT_PAREN,
    AT,
    COLON, SEMICOLON, COMMA,
    DOT,
    QUESTION,
    PLUS, MINUS, STAR, SLASH, PERCENT,
    GREATER, LESS,
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER_EQUAL, LESS_EQUAL,
    AMPERSAND, AMPERSAND_AMPERSAND,
    PIPE, PIPE_PIPE,
    STAR_STAR,
    IDENTIFIER,
    STRING,
    NUMBER,
    STYLE,
    TEXT,
    INHERIT,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    NAMESPACE,
    CONFIGURATION,
    ELEMENT,
    VAR,
    HTML,
    JAVASCRIPT,
    CHTL,
    FROM,
    AS,
    DELETE,
    INSERT,
    AFTER,
    BEFORE,
    REPLACE,
    TOP,
    BOTTOM,
    EXCEPT,
    USE,
    HTML5,
    SCRIPT,
    SYMBOL,
    DOLLAR,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    size_t position; // Starting character position of the lexeme
};

std::string tokenTypeToString(TokenType type);

} // namespace CHTL

#endif // TOKEN_H
