#pragma once

#include <string>
#include <variant>

namespace CHTL {

enum class TokenType {
    // Special Tokens
    ILLEGAL, // An unknown token
    END_OF_FILE,

    // Identifiers and Literals
    IDENTIFIER,         // e.g., div, my_variable
    UNQUOTED_LITERAL,   // e.g., red, 100px
    STRING_LITERAL,     // "hello world"
    NUMBER_LITERAL,     // 100, 3.14

    // Operators
    ASSIGN,             // =
    COLON,              // :
    SEMICOLON,          // ;
    PLUS,               // +
    MINUS,              // -
    ASTERISK,           // *
    SLASH,              // /
    PERCENT,            // %
    POWER,              // **
    AMPERSAND,          // &
    QUESTION,           // ?
    DOT,                // .
    ARROW,              // ->

    // Logical Operators
    AND,                // &&
    OR,                 // ||
    GT,                 // >
    LT,                 // <

    // Delimiters
    LPAREN,             // (
    RPAREN,             // )
    LBRACE,             // {
    RBRACE,             // }
    LBRACKET,           // [
    RBRACKET,           // ]
    COMMA,              // ,

    // Keywords
    TEXT,
    STYLE,
    SCRIPT,
    TEMPLATE,           // [Template]
    CUSTOM,             // [Custom]
    ORIGIN,             // [Origin]
    IMPORT,             // [Import]
    NAMESPACE,          // [Namespace]
    CONFIGURATION,      // [Configuration]
    INFO,               // [Info]
    EXPORT,             // [Export]
    NAME,               // [Name]

    // At-Keywords
    AT_STYLE,           // @Style
    AT_ELEMENT,         // @Element
    AT_VAR,             // @Var
    AT_HTML,            // @Html
    AT_CSS,             // @Css (also handles @style)
    AT_JAVASCRIPT,      // @JavaScript
    AT_CHTL,            // @Chtl
    AT_CJMOD,           // @CJmod
    AT_CONFIG,          // @Config

    // Other Keywords
    INHERIT,
    INSERT,
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM,
    FROM,
    AS,
    EXCEPT,
    USE,
    HTML5,

    // Comments
    COMMENT,             // //, /* */, #
};

struct Token {
    TokenType type;
    std::string literal;
    int line = 0;
    int column = 0;
};

} // namespace CHTL
