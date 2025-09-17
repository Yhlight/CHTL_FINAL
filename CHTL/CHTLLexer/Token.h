#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <map>

namespace CHTL {

// Defines all possible token types in the CHTL language.
enum class TokenType {
    ILLEGAL,       // An unknown or invalid token
    END_OF_FILE,   // EOF

    // --- Identifiers & Literals ---
    IDENTIFIER,    // e.g., div, id, my-class
    STRING,        // "hello world", 'hello world', or unquoted_literal

    // --- Delimiters ---
    LEFT_BRACE,    // {
    RIGHT_BRACE,   // }

    // --- Operators & Punctuation ---
    COLON,         // :
    EQUALS,        // =
    SEMICOLON,     // ;

    // --- Keywords ---
    KEYWORD_TEXT,  // text
    KEYWORD_STYLE, // style (will be more important in Phase 2)
};

// Represents a single token produced by the lexer.
struct Token {
    TokenType type;
    std::string literal; // The actual string value of the token
    int line = 0;        // The line number where the token appears, for error reporting
};

// Helper map to convert token types to strings for debugging.
const std::map<TokenType, std::string> token_type_to_string = {
    {TokenType::ILLEGAL, "ILLEGAL"},
    {TokenType::END_OF_FILE, "EOF"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::STRING, "STRING"},
    {TokenType::LEFT_BRACE, "{"},
    {TokenType::RIGHT_BRACE, "}"},
    {TokenType::COLON, ":"},
    {TokenType::EQUALS, "="},
    {TokenType::SEMICOLON, ";"},
    {TokenType::KEYWORD_TEXT, "text"},
    {TokenType::KEYWORD_STYLE, "style"},
};

} // namespace CHTL

#endif // TOKEN_H
