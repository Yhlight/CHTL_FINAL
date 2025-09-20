#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Single-character tokens.
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE,     // { }
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]
    LEFT_PAREN, RIGHT_PAREN,     // ( )
    AT,                          // @
    COLON, SEMICOLON,            // : ;
    DOT,                         // .
    QUESTION,                    // ?
    COMMA,                       // ,
    PLUS, MINUS, STAR, SLASH,    // + - * /
    PERCENT,                     // %
    GREATER, LESS,               // > <
    DOLLAR,                      // $

    // One or two character tokens
    BANG, BANG_EQUAL,            // ! !=
    EQUAL, EQUAL_EQUAL,          // = ==
    GREATER_EQUAL, LESS_EQUAL,    // >= <=
    AMPERSAND, AMPERSAND_AMPERSAND, // & &&
    PIPE, PIPE_PIPE,             // | ||
    ARROW,                       // ->
    EVENT_BIND,                  // &->

    // Two-character tokens
    STAR_STAR,                   // **
    LEFT_BRACE_BRACE, RIGHT_BRACE_BRACE, // {{ }}

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,
    COMMENT,

    // Keywords
    STYLE,
    TEXT,
    SCRIPT,
    INHERIT,
    FROM,
    AS,
    DELETE,
    INSERT,
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM,
    USE,
    HTML5,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    CONFIGURATION,
    NAMESPACE,
    EXCEPT,
    NAME_GROUP, // [Name]
    ORIGIN_TYPE_GROUP, // [OriginType]

    // Special
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int position;
};

std::string tokenTypeToString(TokenType type);

} // namespace CHTL

#endif // TOKEN_H
