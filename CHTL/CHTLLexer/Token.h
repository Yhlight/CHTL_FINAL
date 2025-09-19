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
    DOT,                         // .
    QUESTION,                    // ?
    COMMA,                       // ,
    PLUS, MINUS, STAR, SLASH, PERCENT,
    GREATER, LESS,
    HASH,                        // #
    DOLLAR,                      // $

    // One or two character tokens
    BANG, BANG_EQUAL,            // ! !=
    EQUAL, EQUAL_EQUAL,          // = ==
    GREATER_EQUAL, LESS_EQUAL,    // >= <=
    AMPERSAND, AMPERSAND_AMPERSAND, // & &&
    PIPE, PIPE_PIPE,             // | ||
    ARROW,                       // ->

    // Two-character tokens
    STAR_STAR,
    LEFT_BRACE_BRACE, RIGHT_BRACE_BRACE, // {{ }}

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,
    UNQUOTED_LITERAL,

    // Keywords
    KEYWORD_STYLE,
    KEYWORD_TEXT,
    KEYWORD_SCRIPT,
    KEYWORD_INHERIT,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_AT_TOP,
    KEYWORD_AT_BOTTOM,
    KEYWORD_EXCEPT,
    KEYWORD_USE,
    KEYWORD_HTML5,

    // Block Keywords
    KEYWORD_TEMPLATE,    // [Template]
    KEYWORD_CUSTOM,      // [Custom]
    KEYWORD_ORIGIN,      // [Origin]
    KEYWORD_IMPORT,      // [Import]
    KEYWORD_NAMESPACE,   // [Namespace]
    KEYWORD_CONFIGURATION, // [Configuration]
    KEYWORD_INFO,        // [Info]
    KEYWORD_EXPORT,      // [Export]

    // Special
    SYMBOL,
    END_OF_FILE,
    UNKNOWN,
    COMMENT // For // and /**/
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
