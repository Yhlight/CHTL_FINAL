#pragma once

#include <string>
#include <variant>

namespace CHTL {

enum class TokenType {
    // Special Tokens
    ILLEGAL, // Represents a token/character we don't know about
    EOF_TOKEN,     // End of File

    // Identifiers & Literals
    IDENTIFIER,     // main, add, foobar, x, y, ...
    STRING_LITERAL, // "foobar" or 'foobar'
    NUMBER_LITERAL, // 123, 10.5

    // Operators and Delimiters
    COLON,          // :
    EQUALS,         // =
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    AMPERSAND,      // &
    QUESTION,       // ?
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    LBRACKET,       // [
    RBRACKET,       // ]

    // Arithmetic Operators
    PLUS,           // +
    MINUS,          // -
    ASTERISK,       // *
    SLASH,          // /
    PERCENT,        // %
    POWER,          // **

    // Logical Operators
    LOGICAL_AND,    // &&
    LOGICAL_OR,     // ||

    // Comments
    LINE_COMMENT,   // //
    BLOCK_COMMENT,  // /* */
    GEN_COMMENT,    // --

    // Keywords
    TEXT,
    STYLE,
    SCRIPT,
    INHERIT,
    DELETE,
    INSERT,
    AFTER,
    BEFORE,
    REPLACE,
    AT,
    TOP,
    BOTTOM,
    FROM,
    AS,
    EXCEPT,
    USE,
    HTML5,

    // Type Prefixes (@...)
    TYPE_STYLE,
    TYPE_ELEMENT,
    TYPE_VAR,
    TYPE_HTML,
    TYPE_CSS,
    TYPE_JAVASCRIPT,
    TYPE_CHTL,
    TYPE_CJMOD,
    TYPE_CONFIG,
    TYPE_VUE, // Example for custom type

    // Bracketed Keywords ([...])
    KEYWORD_CUSTOM,
    KEYWORD_TEMPLATE,
    KEYWORD_ORIGIN,
    KEYWORD_IMPORT,
    KEYWORD_NAMESPACE,
    KEYWORD_CONFIGURATION,
    KEYWORD_INFO,
    KEYWORD_EXPORT,
    KEYWORD_NAME,
    KEYWORD_ORIGINTYPE
};

struct Token {
    TokenType type;
    std::string literal; // The actual characters of the token
    int line;            // Line number for error reporting
};

} // namespace CHTL
