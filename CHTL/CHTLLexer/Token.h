#pragma once

#include <string>
#include <utility>

namespace CHTL {

enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unexpected,

    // Literals
    Identifier,
    StringLiteral,
    UnquotedLiteral,
    Number,

    // Operators
    Plus,           // +
    Minus,          // -
    Star,           // *
    Slash,          // /
    Percent,        // %
    DoubleStar,     // **

    // Punctuation
    OpenBrace,      // {
    CloseBrace,     // }
    Colon,          // :
    Semicolon,      // ;
    Equals,         // =
    OpenParen,      // (
    CloseParen,     // )
    OpenBracket,    // [
    CloseBracket,   // ]
    At,             // @
    Hash,           // #
    Dot,            // .
    Comma,          // ,
    Ampersand,      // &

    // Comments
    LineComment,    // //
    BlockComment,   // /* ... */
    GeneratorComment, // --

    // Keywords (will be expanded later)
    Text,
    Style,
    Script
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
};

} // namespace CHTL
