#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Markers
    ILLEGAL,
    END_OF_FILE,

    // Literals
    IDENTIFIER,         // e.g., div, myVar
    STRING_LITERAL,     // "hello" or 'hello'
    UNQUOTED_LITERAL,   // hello (when used as a value)

    // Symbols
    LBRACE,     // {
    RBRACE,     // }
    LPAREN,     // (
    RPAREN,     // )
    LBRACKET,   // [
    RBRACKET,   // ]
    COLON,      // :
    EQUAL,      // =
    SEMICOLON,  // ;
    COMMA,      // ,
    DOT,        // .
    AT,         // @
    HASH,       // #
    AMPERSAND,  // &
    QUESTION,   // ?
    GT,         // >
    LT,         // <
    SLASH,      // /

    // Operators (for expressions)
    PLUS,       // +
    MINUS,      // -
    ASTERISK,   // *
    LOGICAL_AND,// &&
    LOGICAL_OR, // ||

    // Comments
    LINE_COMMENT,       // //
    BLOCK_COMMENT,      // /* ... */
    GENERATOR_COMMENT,  // --

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

    // Bracketed Keywords
    KEYWORD_CUSTOM,         // [Custom]
    KEYWORD_TEMPLATE,       // [Template]
    KEYWORD_ORIGIN,         // [Origin]
    KEYWORD_IMPORT,         // [Import]
    KEYWORD_NAMESPACE,      // [Namespace]
    KEYWORD_CONFIGURATION,  // [Configuration]
    KEYWORD_INFO,           // [Info]
    KEYWORD_EXPORT,         // [Export]
    KEYWORD_NAME,           // [Name]
    KEYWORD_ORIGINTYPE,     // [OriginType]

    // Type Prefixes (used with keywords)
    TYPE_STYLE,     // @Style
    TYPE_ELEMENT,   // @Element
    TYPE_VAR,       // @Var
    TYPE_HTML,      // @Html
    TYPE_JAVASCRIPT,// @JavaScript
    TYPE_CHTL,      // @Chtl
    TYPE_CJMOD,     // @CJmod
    TYPE_CONFIG,    // @Config
    TYPE_VUE,       // @Vue (example of custom type)
};

struct Token {
    TokenType type = TokenType::ILLEGAL;
    std::string lexeme;
    int line = 0;
    int column = 0;

    // For debugging and logging
    std::string toString() const {
        return "Line " + std::to_string(line) + ":" + std::to_string(column) +
               " | Type: " + std::to_string(static_cast<int>(type)) +
               " | Lexeme: '" + lexeme + "'";
    }
};

} // namespace CHTL
