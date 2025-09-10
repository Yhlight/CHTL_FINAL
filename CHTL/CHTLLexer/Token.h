#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unexpected,

    // Literals
    Identifier,         // e.g., div, my-class, text
    StringLiteral,      // "hello world" or 'hello world'
    UnquotedLiteral,    // wassup

    // Symbols
    OpenBrace,          // {
    CloseBrace,         // }
    OpenParen,          // (
    CloseParen,         // )
    OpenBracket,        // [
    CloseBracket,       // ]
    Colon,              // :
    Semicolon,          // ;
    Equals,             // =
    At,                 // @
    Hash,               // #
    Dot,                // .
    Ampersand,          // &
    Comma,              // ,

    // Operators for expressions
    Plus,               // +
    Minus,              // -
    Asterisk,           // *
    Slash,              // /
    Percent,            // %
    DoubleAsterisk,     // **
    LogicalAnd,         // &&
    LogicalOr,          // ||
    QuestionMark,       // ?


    // Comments
    SingleLineComment,  // //
    MultiLineComment,   // /* ... */
    GeneratorComment,   // --

    // Keywords (will be handled as identifiers initially and resolved in parser)
    // For now, we just need a generic identifier.
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

// Helper function for debugging
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile: return "EndOfFile";
        case TokenType::Unexpected: return "Unexpected";
        case TokenType::Identifier: return "Identifier";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::UnquotedLiteral: return "UnquotedLiteral";
        case TokenType::OpenBrace: return "OpenBrace";
        case TokenType::CloseBrace: return "CloseBrace";
        case TokenType::OpenParen: return "OpenParen";
        case TokenType::CloseParen: return "CloseParen";
        case TokenType::OpenBracket: return "OpenBracket";
        case TokenType::CloseBracket: return "CloseBracket";
        case TokenType::Colon: return "Colon";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Equals: return "Equals";
        case TokenType::At: return "At";
        case TokenType::Hash: return "Hash";
        case TokenType::Dot: return "Dot";
        case TokenType::Ampersand: return "Ampersand";
        case TokenType::Comma: return "Comma";
        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Asterisk: return "Asterisk";
        case TokenType::Slash: return "Slash";
        case TokenType::Percent: return "Percent";
        case TokenType::DoubleAsterisk: return "DoubleAsterisk";
        case TokenType::LogicalAnd: return "LogicalAnd";
        case TokenType::LogicalOr: return "LogicalOr";
        case TokenType::QuestionMark: return "QuestionMark";
        case TokenType::SingleLineComment: return "SingleLineComment";
        case TokenType::MultiLineComment: return "MultiLineComment";
        case TokenType::GeneratorComment: return "GeneratorComment";
        default: return "Unknown";
    }
}
