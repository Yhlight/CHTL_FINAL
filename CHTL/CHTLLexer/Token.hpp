#pragma once

#include <string>
#include <variant>

namespace CHTL {

enum class TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]
    AT, // @
    COLON, EQUAL, SEMICOLON,
    LEFT_PAREN, RIGHT_PAREN,

    // One or two character tokens.
    MINUS, PLUS, SLASH, STAR, PERCENT, STAR_STAR,

    // Literals.
    IDENTIFIER,
    STRING, // For "...", '...', and unquoted literals

    // Keywords
    TEMPLATE,
    CUSTOM,
    IMPORT,
    NAMESPACE,
    FROM,
    AS,
    DELETE,
    INSERT,
    AFTER,
    BEFORE,

    // Comments
    LINE_COMMENT, // //
    BLOCK_COMMENT, // /* ... */
    GENERATOR_COMMENT, // --

    // End of file.
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

} // namespace CHTL
