#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unknown,

    // Literals
    Identifier,         // e.g., div, id, width
    UnquotedLiteral,    // e.g., red, 100px
    StringLiteral,      // e.g., "hello world"

    // Symbols
    LeftBrace,          // {
    RightBrace,         // }
    LeftParen,          // (
    RightParen,         // )
    LeftBracket,        // [
    RightBracket,       // ]
    Colon,              // : or =
    Semicolon,          // ;
    At,                 // @
    Hash,               // #
    Dot,                // .
    Comma,              // ,
    Ampersand,          // &
    GreaterThan,        // >
    LessThan,           // <

    // Keywords
    Text,               // text
    Style,              // style
    TemplateKeyword,    // [Template]
    NamespaceKeyword,   // [Namespace]
    ImportKeyword,      // [Import]
    FromKeyword,        // from
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    size_t offset; // The starting offset in the source string

    // For debugging
    std::string toString() const;
};

#endif // TOKEN_H
