#ifndef CHTL_JS_TOKEN_H
#define CHTL_JS_TOKEN_H

#include <string>

namespace CHTLJS {

enum class CHTLJSTokenType {
    // Special
    EndOfFile,
    Unexpected,

    // Punctuation
    OpenBrace,        // {
    CloseBrace,       // }
    OpenBracket,      // [
    CloseBracket,     // ]
    Colon,            // :
    Comma,            // ,
    Identifier,
    Selector,
    String,
    Number,
    Dot,              // .
    Arrow,            // ->
    Equals,           // =
    Semicolon,        // ;

    // Keywords (for future features)
    Listen,
    Animate,
    Delegate,
    Router,
    Vir,
};

struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string lexeme;
    size_t line = 1;
    size_t column = 1;
};

} // namespace CHTLJS

#endif // CHTL_JS_TOKEN_H
