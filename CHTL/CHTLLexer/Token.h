#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Special Tokens
    ILLEGAL,
    END_OF_FILE,

    // Identifiers & Literals
    IDENTIFIER,         // e.g., div, my_class, width
    STRING_LITERAL,     // "hello world" or 'hello world'
    NUMERIC_LITERAL,    // 100, 1.6, 50px (will need post-processing)
    UNQUOTED_LITERAL,   // red, solid, 100px (treated as a single unit)

    // Operators
    COLON,              // :
    EQUALS,             // =
    SEMICOLON,          // ;
    PLUS,               // +
    MINUS,              // -
    ASTERISK,           // *
    SLASH,              // /
    PERCENT,            // %
    DOUBLE_ASTERISK,    // **
    QUESTION_MARK,      // ?
    AMPERSAND,          // &
    DOUBLE_AMPERSAND,   // &&
    PIPE,               // |
    DOUBLE_PIPE,        // ||

    // Delimiters
    LBRACE,             // {
    RBRACE,             // }
    LPAREN,             // (
    RPAREN,             // )
    LBRACKET,           // [
    RBRACKET,           // ]
    COMMA,              // ,
    DOT,                // .
    HASH,               // #

    // Comments
    SINGLE_LINE_COMMENT, // // ...
    MULTI_LINE_COMMENT,  // /* ... */
    GENERATOR_COMMENT,   // # ...

    // Keywords (will be identified from IDENTIFIER)
    // CHTL structure
    TEXT,
    STYLE,
    SCRIPT,

    // Templates & Customization
    KEYWORD_TEMPLATE,   // [Template] // This was already here, but not used.
    KEYWORD_CUSTOM,     // [Custom]
    KEYWORD_ORIGIN,     // [Origin]
    KEYWORD_IMPORT,     // [Import]
    KEYWORD_NAMESPACE,  // [Namespace]
    KEYWORD_CONFIG,     // [Configuration]

    // Template/Custom Types
    AT_STYLE,           // @Style
    AT_ELEMENT,         // @Element
    AT_VAR,             // @Var
    AT_HTML,            // @Html
    AT_CSS,             // @Css
    AT_JAVASCRIPT,      // @JavaScript
    AT_CHTL,            // @Chtl
    AT_CJMOD,           // @CJmod
    AT_CONFIG,          // @Config

    // Other Keywords
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
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};
