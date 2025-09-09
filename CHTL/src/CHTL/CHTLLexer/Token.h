#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Special tokens
    EndOfFile,
    Unexpected,

    // Literals
    Identifier,
    StringLiteral,
    UnquotedLiteral,

    // Punctuation
    OpenBrace,
    CloseBrace,
    OpenParen,
    CloseParen,
    Comma,
    Semicolon,
    LeftBracket,
    RightBracket,
    At,

    // Keywords
    KeywordDelete,
    KeywordInsert,
    KeywordAfter,
    KeywordBefore,
    KeywordReplace,
    KeywordAtTop,
    KeywordAtBottom,
    KeywordFrom,
    KeywordAs,
    KeywordInherit,
    // Note: [Import] and [Namespace] are parsed as bracket + identifier

    // Style-specific tokens
    Colon,
    ColonColon,
    Equals,
    Dot,
    Hash,
    Ampersand,

    // Expression operators
    QuestionMark,
    GreaterThan,
    LessThan,
    Plus,
    Minus,
    Star,
    Slash,
    LogicalAnd,
    LogicalOr,

    // Comments
    LineComment,
    BlockComment,
    GeneratorComment,
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    size_t start = 0;
    size_t end = 0;
};

} // namespace CHTL
