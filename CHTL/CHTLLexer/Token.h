#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

enum class TokenType {
    // Special Tokens
    ERROR,
    END_OF_FILE,

    // Single-character tokens
    LEFT_BRACE,    // {
    RIGHT_BRACE,   // }
    LEFT_PAREN,    // (
    RIGHT_PAREN,   // )
    LEFT_BRACKET,  // [
    RIGHT_BRACKET, // ]
    COLON,         // :
    SEMICOLON,     // ;
    COMMA,         // ,
    DOT,           // .
    PLUS,          // +
    MINUS,         // -
    SLASH,         // /
    STAR,          // *
    PERCENT,       // %
    AMPERSAND,     // &
    PIPE,          // |
    QUESTION,      // ?
    EQUAL,         // =
    LESS,          // <
    GREATER,       // >
    BANG,          // !
    HASH,          // #
    AT,            // @

    // Two-character tokens
    STAR_STAR,           // **
    BANG_EQUAL,          // !=
    EQUAL_EQUAL,         // ==
    LESS_EQUAL,          // <=
    GREATER_EQUAL,       // >=
    AMPERSAND_AMPERSAND, // &&
    PIPE_PIPE,           // ||
    MINUS_GREATER,       // ->

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,
    UNQUOTED_LITERAL, // A key feature of CHTL

    // Keywords
    // Note: Many keywords are contextual. The lexer will tokenize them as IDENTIFIERs
    // or specific keywords, and the parser will validate the context.
    TEXT,
    STYLE,
    SCRIPT,
    INHERIT,
    DELETE,
    INSERT,
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM,
    FROM,
    AS,
    EXCEPT,
    USE,
    HTML5,

    // CHTL Keywords with brackets/at-symbols that might need special handling
    KEYWORD_CUSTOM,      // [Custom]
    KEYWORD_TEMPLATE,    // [Template]
    KEYWORD_ORIGIN,      // [Origin]
    KEYWORD_IMPORT,      // [Import]
    KEYWORD_NAMESPACE,   // [Namespace]
    KEYWORD_CONFIG,      // [Configuration]
    KEYWORD_INFO,        // [Info]
    KEYWORD_EXPORT,      // [Export]
    KEYWORD_NAME,        // [Name]
    KEYWORD_ORIGIN_TYPE, // [OriginType]

    // Comments (usually skipped, but can be a token for specific cases)
    COMMENT,
};

// Function to convert TokenType to a string for debugging
std::string tokenTypeToString(TokenType type);

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    std::string toString() const;
};

#endif // TOKEN_H
