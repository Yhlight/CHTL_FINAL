#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Structural tokens
    LBRACE,         // {
    RBRACE,         // }
    COLON,          // :
    EQUALS,         // =
    SEMICOLON,      // ;
    L_BRACKET,      // [
    R_BRACKET,      // ]
    L_PAREN,        // (
    R_PAREN,        // )
    COMMA,          // ,
    AT_SIGN,        // @
    DOT,            // .
    HASH,           // #
    AMPERSAND,      // &

    // Operators
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /

    // Literals
    TAG_NAME,       // e.g., div, p, html
    IDENTIFIER,     // General identifier
    STRING_LITERAL, // "a string" or 'a string'
    UNQUOTED_LITERAL, // a_bare_word
    NUMBER_LITERAL, // 123, 1.5

    // Keywords
    KEYWORD_TEXT,   // text
    KEYWORD_STYLE,  // style

    // Comments
    COMMENT,        // // a comment or /* a comment */

    // Special
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
};

// Helper function for debugging
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::L_BRACKET: return "L_BRACKET";
        case TokenType::R_BRACKET: return "R_BRACKET";
        case TokenType::L_PAREN: return "L_PAREN";
        case TokenType::R_PAREN: return "R_PAREN";
        case TokenType::COMMA: return "COMMA";
        case TokenType::AT_SIGN: return "AT_SIGN";
        case TokenType::DOT: return "DOT";
        case TokenType::HASH: return "HASH";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::TAG_NAME: return "TAG_NAME";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case TokenType::KEYWORD_TEXT: return "KEYWORD_TEXT";
        case TokenType::KEYWORD_STYLE: return "KEYWORD_STYLE";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN_TYPE";
    }
}
