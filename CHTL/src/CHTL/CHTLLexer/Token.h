#pragma once

#include <string>
#include <vector>

namespace CHTL {

// Enum for all possible token types in the CHTL language.
enum class TokenType {
    // Special tokens
    EndOfFile,
    Unexpected,

    // Literals
    Identifier,         // e.g., div, id, color, background-color
    StringLiteral,      // e.g., "hello world"
    UnquotedLiteral,    // e.g., red, 100px (Currently handled by Identifier, may be split later)

    // Punctuation
    OpenBrace,          // {
    CloseBrace,         // }
    Semicolon,          // ;

    // Style-specific tokens
    Colon,              // :
    ColonColon,         // ::
    Equals,             // =
    Dot,                // .
    Hash,               // #
    Ampersand,          // &

    // Comments (Note: The lexer will skip these, but the types are defined for potential future use)
    LineComment,
    BlockComment,
    GeneratorComment,
};

// Represents a single token extracted from the source code.
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

} // namespace CHTL
