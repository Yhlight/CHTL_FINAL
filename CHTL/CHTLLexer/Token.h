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

    // Comparison
    DoubleEquals,   // ==
    NotEquals,      // !=
    GreaterThan,    // >
    LessThan,       // <
    GreaterThanOrEquals, // >=
    LessThanOrEquals,    // <=

    // Logical
    LogicalAnd,     // &&
    LogicalOr,      // ||

    // Ternary
    QuestionMark,   // ?

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

    // Keywords
    Text,
    Style,
    Script,
    Delete          // delete keyword
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
};

} // namespace CHTL
