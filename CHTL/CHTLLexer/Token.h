#pragma once

#include <string>
#include <vector>

namespace CHTL {

// Defines the different types of tokens that the lexer can produce.
enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unexpected,

    // Literals
    Identifier,      // e.g., div, id, text
    StringLiteral,   // e.g., "hello world" or 'hello world' or helloworld

    // Punctuation
    OpenBrace,       // {
    CloseBrace,      // }
    Colon,           // :
    Semicolon,       // ;
    Equals,          // = (CE equality)

    // Comments
    LineComment,     // // a comment
    BlockComment,    // /* a comment */
    GeneratorComment, // -- a comment

    // CSS-specific
    Dot,             // .
    Hash,            // #
    Ampersand        // &
};

// Represents a single token extracted from the source code.
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

// Helper function to convert a TokenType to a string for debugging purposes.
inline std::string TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile: return "EndOfFile";
        case TokenType::Unexpected: return "Unexpected";
        case TokenType::Identifier: return "Identifier";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::OpenBrace: return "OpenBrace";
        case TokenType::CloseBrace: return "CloseBrace";
        case TokenType::Colon: return "Colon";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Equals: return "Equals";
        case TokenType::LineComment: return "LineComment";
        case TokenType::BlockComment: return "BlockComment";
        case TokenType::GeneratorComment: return "GeneratorComment";
        case TokenType::Dot: return "Dot";
        case TokenType::Hash: return "Hash";
        case TokenType::Ampersand: return "Ampersand";
        default: return "Unknown";
    }
}

} // namespace CHTL
