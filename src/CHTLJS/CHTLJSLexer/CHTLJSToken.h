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
    Placeholder,
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
};

struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string lexeme;
    size_t line;
    size_t column;
    size_t start_pos; // Start character offset in the source
    size_t end_pos;   // End character offset in the source
};

} // namespace CHTLJS

#endif // CHTL_JS_TOKEN_H
