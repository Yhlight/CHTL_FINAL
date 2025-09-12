#pragma once

#include <string>
#include <vector>
#include <map>

namespace CHTL {

enum class TokenType {
    // Special Tokens
    ILLEGAL,
    END_OF_FILE,

    // Identifiers & Literals
    IDENTIFIER,     // e.g., div, my_variable, text
    STRING,         // "string", 'string', or unquoted_string

    // Symbols & Operators
    LBRACE,         // {
    RBRACE,         // }
    LPAREN,         // (
    RPAREN,         // )
    LBRACKET,       // [
    RBRACKET,       // ]
    COLON,          // :
    EQUALS,         // =
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    AMPERSAND,      // &
    QUESTION,       // ?
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /
    PERCENT,        // %

    // Generator-aware Comment
    COMMENT_GEN,    // -- a comment
};

// Map for converting TokenType to a string for debugging
const std::map<TokenType, std::string> token_type_strings = {
    {TokenType::ILLEGAL, "ILLEGAL"},
    {TokenType::END_OF_FILE, "EOF"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::STRING, "STRING"},
    {TokenType::LBRACE, "LBRACE"},
    {TokenType::RBRACE, "RBRACE"},
    {TokenType::LPAREN, "LPAREN"},
    {TokenType::RPAREN, "RPAREN"},
    {TokenType::LBRACKET, "LBRACKET"},
    {TokenType::RBRACKET, "RBRACKET"},
    {TokenType::COLON, "COLON"},
    {TokenType::EQUALS, "EQUALS"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::DOT, "DOT"},
    {TokenType::AMPERSAND, "AMPERSAND"},
    {TokenType::QUESTION, "QUESTION"},
    {TokenType::PLUS, "PLUS"},
    {TokenType::MINUS, "MINUS"},
    {TokenType::STAR, "STAR"},
    {TokenType::SLASH, "SLASH"},
    {TokenType::PERCENT, "PERCENT"},
    {TokenType::COMMENT_GEN, "COMMENT_GEN"},
};

inline std::string TokenTypeToString(TokenType type) {
    auto it = token_type_strings.find(type);
    if (it != token_type_strings.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

struct Token {
    TokenType type = TokenType::ILLEGAL;
    std::string literal;
    int line = 0;
    int column = 0;
};

} // namespace CHTL
