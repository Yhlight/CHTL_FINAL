#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace CHTL {

enum class TokenType {
    // End of file
    END_OF_FILE,

    // Identifiers
    IDENTIFIER,         // e.g., div, id, color

    // Literals
    STRING_LITERAL,     // "hello world" or 'hello world'

    // This will be handled by the parser reading raw content, but we can have a token for it.
    UNQUOTED_LITERAL,   // hello_world (without quotes)

    // Delimiters
    L_BRACE,            // {
    R_BRACE,            // }
    L_PAREN,            // (
    R_PAREN,            // )
    L_BRACKET,          // [
    R_BRACKET,          // ]

    // Operators and Punctuation
    COLON,              // :
    EQUALS,             // =
    SEMICOLON,          // ;
    COMMA,              // ,
    AT,                 // @
    HASH,               // #
    DOT,                // .
    AMPERSAND,          // &
    QUESTION,           // ?

    // Comments
    SINGLE_LINE_COMMENT, // // a comment
    MULTI_LINE_COMMENT,  // /* a comment */
    GENERATOR_COMMENT,   // -- a comment

    // Unknown
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

} // namespace CHTL

#endif // TOKEN_H
