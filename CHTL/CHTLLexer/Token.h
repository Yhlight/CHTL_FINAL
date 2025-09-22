#pragma once

#include <string>
#include <vector>

namespace CHTL {

// Enum to represent all possible token types in the CHTL language.
enum class TokenType {
    // Special Tokens
    ILLEGAL,        // Represents a token/character we don't know about
    END_OF_FILE,

    // Delimiters
    LBRACE,         // {
    RBRACE,         // }

    // Literals & Identifiers
    IDENTIFIER,     // main, div, text, etc. Also used for unquoted literals for now.
    STRING,         // "a string" or 'a string'
};

// Helper function for debugging to convert a TokenType to its string representation.
inline std::string TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::ILLEGAL:        return "ILLEGAL";
        case TokenType::END_OF_FILE:    return "EOF";
        case TokenType::LBRACE:         return "LBRACE";
        case TokenType::RBRACE:         return "RBRACE";
        case TokenType::IDENTIFIER:     return "IDENTIFIER";
        case TokenType::STRING:         return "STRING";
        default:                        return "UNKNOWN";
    }
}

// Represents a single token produced by the Lexer.
struct Token {
    TokenType type;
    std::string value;
};

} // namespace CHTL
