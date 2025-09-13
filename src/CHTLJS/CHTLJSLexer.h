#ifndef CHTL_JS_LEXER_H
#define CHTL_JS_LEXER_H

#include <string>
#include <vector>
#include <variant>

namespace CHTL {
namespace CHTLJS {

enum class JSTokenType {
    StandardJS,
    EnhancedSelector,   // {{...}}
    ListenBlock,        // listen {...}
    ScriptLoaderBlock,  // scriptloader {...}
    Arrow,              // ->
    EndOfFile
};

struct JSToken {
    JSTokenType type;
    std::string content;
};

class CHTLJSLexer {
public:
    explicit CHTLJSLexer(const std::string& source);
    std::vector<JSToken> scanTokens();

private:
    void findNextToken();

    const std::string& source_;
    size_t current_ = 0;
    std::vector<JSToken> tokens_;
};

} // namespace CHTLJS
} // namespace CHTL

#endif // CHTL_JS_LEXER_H
