#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Special Tokens
    TOKEN_ILLEGAL, // An unknown or illegal token
    TOKEN_EOF,     // End of File

    // Identifiers and Literals
    TOKEN_IDENTIFIER, // e.g., div, myVar, red
    TOKEN_STRING,     // "a string" or 'a string'
    TOKEN_NUMBER,     // 100, 3.14

    // Operators
    TOKEN_EQUAL,       // =
    TOKEN_COLON,       // :
    TOKEN_PLUS,        // +
    TOKEN_MINUS,       // -
    TOKEN_STAR,        // *
    TOKEN_SLASH,       // /
    TOKEN_PERCENT,     // %
    TOKEN_POWER,       // **
    TOKEN_QUESTION,    // ?
    TOKEN_LOGICAL_AND, // &&
    TOKEN_LOGICAL_OR,  // ||
    TOKEN_AMPERSAND,   // &

    // Delimiters
    TOKEN_LPAREN,   // (
    TOKEN_RPAREN,   // )
    TOKEN_LBRACE,   // {
    TOKEN_RBRACE,   // }
    TOKEN_LBRACKET, // [
    TOKEN_RBRACKET, // ]
    TOKEN_COMMA,    // ,
    TOKEN_SEMICOLON,// ;
    TOKEN_DOT,      // .
    TOKEN_HASH,     // #
    TOKEN_AT,       // @

    // Keywords for CHTL structure and types
    TOKEN_KEYWORD_CUSTOM,      // [Custom]
    TOKEN_KEYWORD_TEMPLATE,    // [Template]
    TOKEN_KEYWORD_ORIGIN,      // [Origin]
    TOKEN_KEYWORD_IMPORT,      // [Import]
    TOKEN_KEYWORD_NAMESPACE,   // [Namespace]
    TOKEN_KEYWORD_CONFIGURATION, // [Configuration]
    TOKEN_KEYWORD_INFO,        // [Info]
    TOKEN_KEYWORD_EXPORT,      // [Export]
    TOKEN_KEYWORD_NAME,        // [Name]
    TOKEN_KEYWORD_ORIGINTYPE,  // [OriginType]

    // Keywords for types (@-prefixed)
    TOKEN_KEYWORD_STYLE,    // style, @Style
    TOKEN_KEYWORD_ELEMENT,  // @Element
    TOKEN_KEYWORD_VAR,      // @Var
    TOKEN_KEYWORD_HTML,     // @Html
    TOKEN_KEYWORD_JAVASCRIPT, // @JavaScript
    TOKEN_KEYWORD_CHTL,     // @Chtl
    TOKEN_KEYWORD_CJMOD,    // @CJmod
    TOKEN_KEYWORD_CONFIG,   // @Config

    // General Keywords
    TOKEN_KEYWORD_TEXT,     // text
    TOKEN_KEYWORD_SCRIPT,   // script
    TOKEN_KEYWORD_INHERIT,  // inherit
    TOKEN_KEYWORD_DELETE,   // delete
    TOKEN_KEYWORD_INSERT,   // insert
    TOKEN_KEYWORD_AFTER,    // after
    TOKEN_KEYWORD_BEFORE,   // before
    TOKEN_KEYWORD_REPLACE,  // replace
    TOKEN_KEYWORD_ATTOP,    // at top
    TOKEN_KEYWORD_ATBOTTOM, // at bottom
    TOKEN_KEYWORD_FROM,     // from
    TOKEN_KEYWORD_AS,       // as
    TOKEN_KEYWORD_EXCEPT,   // except
    TOKEN_KEYWORD_USE,      // use
    TOKEN_KEYWORD_HTML5,    // html5

    // Comments
    TOKEN_COMMENT, // # a comment
};

// Helper function to convert token type to string for debugging
std::string tokenTypeToString(TokenType type);

struct Token {
    TokenType type;
    std::string literal;
    int line;
    int column;

    Token(TokenType t = TokenType::TOKEN_ILLEGAL, std::string l = "", int ln = 0, int col = 0)
        : type(t), literal(l), line(ln), column(col) {}
};

} // namespace CHTL
