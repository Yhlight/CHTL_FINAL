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
};

} // namespace CHTL
