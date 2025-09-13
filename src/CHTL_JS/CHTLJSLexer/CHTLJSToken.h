#ifndef CHTL_JS_TOKEN_H
#define CHTL_JS_TOKEN_H

#include <string>

namespace CHTL {
namespace CHTL_JS {

enum class JSTokenType {
    EndOfFile,
    Unexpected,
    Selector,       // Represents a full {{...}} expression
    JSCode,         // Represents a __JS_CODE_...__ placeholder
};

struct JSToken {
    JSTokenType type;
    std::string text;
};

} // namespace CHTL_JS
} // namespace CHTL

#endif // CHTL_JS_TOKEN_H
