#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <iostream>

namespace CHTL {

enum class TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE, COLON, EQUALS, SEMICOLON,

    // Literals.
    IDENTIFIER, STRING,

    // Keywords
    KEYWORD_TEXT,

    // Comments
    COMMENT,

    // End of file.
    END_OF_FILE,

    // Error/Unknown
    UNKNOWN
};

// Helper to print token types for debugging
inline std::ostream& operator<<(std::ostream& os, const TokenType& type) {
    switch (type) {
        case TokenType::LEFT_BRACE: os << "LEFT_BRACE"; break;
        case TokenType::RIGHT_BRACE: os << "RIGHT_BRACE"; break;
        case TokenType::COLON: os << "COLON"; break;
        case TokenType::EQUALS: os << "EQUALS"; break;
        case TokenType::SEMICOLON: os << "SEMICOLON"; break;
        case TokenType::IDENTIFIER: os << "IDENTIFIER"; break;
        case TokenType::STRING: os << "STRING"; break;
        case TokenType::KEYWORD_TEXT: os << "KEYWORD_TEXT"; break;
        case TokenType::COMMENT: os << "COMMENT"; break;
        case TokenType::END_OF_FILE: os << "END_OF_FILE"; break;
        case TokenType::UNKNOWN: os << "UNKNOWN"; break;
    }
    return os;
}

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    void print() const {
        std::cout << "Token(" << type << ", '" << lexeme << "', "
                  << line << ":" << column << ")" << std::endl;
    }
};

} // namespace CHTL

#endif // CHTL_TOKEN_H
