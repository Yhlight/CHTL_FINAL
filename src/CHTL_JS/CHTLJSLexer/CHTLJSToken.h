#ifndef CHTL_JS_TOKEN_H
#define CHTL_JS_TOKEN_H

#include <string>

namespace CHTL {
namespace CHTL_JS {

enum class JSTokenType {
    EndOfFile,
    Unexpected,

    // CHTL JS constructs
    Selector,       // {{...}}
    Listen,         // listen

    // JS placeholers
    JSCode,         // __JS_CODE_...__

    // Primitives
    Identifier,

    // Punctuation
    Arrow,          // ->
    OpenBrace,      // {
    CloseBrace,     // }
    Colon,          // :
    Comma,          // ,
};

struct JSToken {
    JSTokenType type;
    std::string text;
};

} // namespace CHTL_JS
} // namespace CHTL

#endif // CHTL_JS_TOKEN_H
