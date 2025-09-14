#ifndef CHTL_JS_TOKEN_H
#define CHTL_JS_TOKEN_H

#include <string>

namespace CHTLJS {

enum class CHTLJSTokenType {
    // Special
    EndOfFile,
    Unexpected,

    // Punctuation
    OpenDoubleBrace,  // {{
    CloseDoubleBrace, // }}
    OpenBrace,        // {
    CloseBrace,       // }
    OpenBracket,      // [
    CloseBracket,     // ]
    Colon,            // :
    Comma,            // ,
    Identifier,
    String,
    Number,
    Dot,              // .
    Arrow,            // ->
    Equals,           // =

    // Keywords (for future features)
    Listen,
    Animate,
    Delegate,
    Router,
    Vir,

    // Represents a block of pure JS code
    Placeholder,
};

struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string lexeme;
    size_t line = 1;
    size_t column = 1;
};

} // namespace CHTLJS

#endif // CHTL_JS_TOKEN_H
