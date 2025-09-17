#pragma once

#include <string>
#include <vector>

// Enum for all possible token types in CHTL
enum class TokenType {
    // Special Tokens
    ILLEGAL, // Represents a token/character we don't support
    END_OF_FILE,

    // Identifiers & Literals
    IDENTIFIER,   // main, div, id, class
    STRING_LITERAL, // "some text", 'some text'
    UNQUOTED_LITERAL, // some text (without quotes)

    // Delimiters
    LEFT_BRACE,  // {
    RIGHT_BRACE, // }
    COLON,       // :
    SEMICOLON,   // ;
    EQUALS,      // =
    AMPERSAND,   // &
    DOT,         // .
    POUND,       // #
    AT_SIGN,     // @
    LEFT_BRACKET,  // [
    RIGHT_BRACKET, // ]

    // Comments
    HASH_COMMENT, // # A comment

    // Keywords are handled by the parser based on identifier value
};

// A struct to represent a token
struct Token {
    TokenType type;
    std::string literal; // The actual characters of the token
    int line;            // The line number where the token appears

    // Helper for debugging
    std::string toString() const;
};

// Helper function to convert TokenType to a string for printing/debugging
std::string tokenTypeToString(TokenType type);
