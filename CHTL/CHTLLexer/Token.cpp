#include "Token.h"
#include <string>

// Converts a TokenType enum to its string representation.
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::ILLEGAL:            return "ILLEGAL";
        case TokenType::END_OF_FILE:        return "END_OF_FILE";
        case TokenType::IDENTIFIER:         return "IDENTIFIER";
        case TokenType::STRING_LITERAL:     return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL:   return "UNQUOTED_LITERAL";
        case TokenType::LEFT_BRACE:         return "{";
        case TokenType::RIGHT_BRACE:        return "}";
        case TokenType::COLON:              return ":";
        case TokenType::SEMICOLON:          return ";";
        case TokenType::EQUALS:             return "=";
        case TokenType::AMPERSAND:          return "&";
        case TokenType::DOT:                return ".";
        case TokenType::POUND:              return "#";
        case TokenType::HASH_COMMENT:       return "HASH_COMMENT";
        default:                            return "UNKNOWN";
    }
}

// Returns a string representation of a Token, useful for debugging.
std::string Token::toString() const {
    return "Token[type: " + tokenTypeToString(type) + ", literal: '" + literal + "', line: " + std::to_string(line) + "]";
}
