#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    ILLEGAL, // A token/character we don't know about
    END_OF_FILE,

    // Identifiers & Literals
    IDENT, // add, foobar, x, y, ...
    STRING, // "foobar"
    INHERIT, // inherit
    TEXT, // Unquoted literal text
    COMMENT, // # this is a comment

    // Operators and Delimiters
    LBRACE, // {
    RBRACE, // }
    COLON, // :
    EQUAL, // =
    SEMICOLON, // ;
    LBRACKET, // [
    RBRACKET, // ]
    AT, // @
    LPAREN, // (
    RPAREN, // )

    // Literals
    NUMBER, // 12345

    // Operators
    PLUS,     // +
    MINUS,    // -
    ASTERISK, // *
    SLASH,    // /

    // Conditionals
    LT,       // <
    GT,       // >
    QUESTION, // ?
};

// A simple function to convert TokenType to string for debugging
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::ILLEGAL: return "ILLEGAL";
        case TokenType::END_OF_FILE: return "EOF";
        case TokenType::IDENT: return "IDENT";
        case TokenType::STRING: return "STRING";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::TEXT: return "TEXT";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::LBRACE: return "{";
        case TokenType::RBRACE: return "}";
        case TokenType::COLON: return ":";
        case TokenType::EQUAL: return "=";
        case TokenType::SEMICOLON: return ";";
        case TokenType::LBRACKET: return "[";
        case TokenType::RBRACKET: return "]";
        case TokenType::AT: return "@";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::ASTERISK: return "*";
        case TokenType::SLASH: return "/";
        case TokenType::LT: return "<";
        case TokenType::GT: return ">";
        case TokenType::QUESTION: return "?";
        default: return "UNKNOWN";
    }
}

struct Token {
    TokenType type;
    std::string literal;
    int line = 0; // For error reporting
};

} // namespace CHTL
