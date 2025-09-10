#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

enum class TokenType {
    // Special Tokens
    ILLEGAL, // Represents an invalid or unrecognized token
    END_OF_FILE,

    // Identifiers and Literals
    IDENTIFIER,         // e.g., div, my_variable
    STRING_LITERAL,     // "hello world" or 'hello world'
    UNQUOTED_LITERAL,   // hello_world (without quotes)
    NUMBER_LITERAL,     // 100, 3.14

    // Symbols
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    COLON,          // :
    EQUALS,         // =
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    AT,             // @
    HASH,           // #
    DOLLAR,         // $
    AMPERSAND,      // &

    // Operators
    PLUS,           // +
    MINUS,          // -
    ASTERISK,       // *
    SLASH,          // /
    PERCENT,        // %
    POWER,          // **
    LOGICAL_AND,    // &&
    LOGICAL_OR,     // ||
    QUESTION_MARK,  // ?

    // Comments
    SINGLE_LINE_COMMENT,    // // ...
    MULTI_LINE_COMMENT,     // /* ... */
    GENERATOR_COMMENT,      // -- ...

    // Keywords (will be identified from IDENTIFIER)
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_INHERIT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_AT_TOP,
    KEYWORD_AT_BOTTOM,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_EXCEPT,
    KEYWORD_USE,
    KEYWORD_HTML5,

    // Bracketed Keywords
    KEYWORD_CUSTOM,         // [Custom]
    KEYWORD_TEMPLATE,       // [Template]
    KEYWORD_ORIGIN,         // [Origin]
    KEYWORD_IMPORT,         // [Import]
    KEYWORD_NAMESPACE,      // [Namespace]
    KEYWORD_CONFIGURATION,  // [Configuration]
    KEYWORD_INFO,           // [Info]
    KEYWORD_EXPORT,         // [Export]
    KEYWORD_NAME,           // [Name]
    KEYWORD_ORIGIN_TYPE,    // [OriginType]
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    // Helper to convert token type to string for debugging
    std::string typeToString() const;
};

#endif //CHTL_TOKEN_H
