#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,     // { }
    LEFT_PAREN, RIGHT_PAREN,     // ( )
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]
    COMMA, DOT, MINUS, PLUS, STAR,
    SEMICOLON, SLASH, PERCENT,
    COLON, EQUAL,
    QUESTION_MARK,

    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    STAR_STAR, // Power `**`
    CHTL_COMMENT, // `--`
    ARROW, // `->`

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,
    UNQUOTED_LITERAL,

    // Keywords
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_INHERIT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_ATTOP,
    KEYWORD_ATBOTTOM,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_EXCEPT,
    KEYWORD_USE,
    KEYWORD_HTML5,

    // Block Keywords
    KEYWORD_CUSTOM,      // [Custom]
    KEYWORD_TEMPLATE,    // [Template]
    KEYWORD_ORIGIN,      // [Origin]
    KEYWORD_IMPORT,      // [Import]
    KEYWORD_NAMESPACE,   // [Namespace]
    KEYWORD_CONFIGURATION, // [Configuration]
    KEYWORD_INFO,        // [Info]
    KEYWORD_EXPORT,      // [Export]
    KEYWORD_ORIGIN_TYPE, // [OriginType]
    KEYWORD_NAME,        // [Name]

    // Type Keywords (prefixed with @)
    KEYWORD_AT_STYLE,
    KEYWORD_AT_ELEMENT,
    KEYWORD_AT_VAR,
    KEYWORD_AT_HTML,
    KEYWORD_AT_JAVASCRIPT,
    KEYWORD_AT_CHTL,
    KEYWORD_AT_CJMOD,
    KEYWORD_AT_CONFIG,
    // For custom @ types like @Vue
    KEYWORD_AT_CUSTOM,

    // Misc
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType type, const std::string& lexeme, int line, int column);

    std::string toString() const;
};

} // namespace CHTL

#endif // CHTL_TOKEN_H
