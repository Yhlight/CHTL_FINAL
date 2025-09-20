#ifndef CHTLJS_LEXER_H
#define CHTLJS_LEXER_H

#include <string>
#include <vector>

namespace CHTL {

// CHTL JS will have its own set of tokens.
enum class CHTLJSTokenType {
    // ... to be defined ...
    ENHANCED_SELECTOR, // {{...}}
    LISTEN_KEYWORD,    // Listen
    DELEGATE_KEYWORD,  // Delegate
    ANIMATE_KEYWORD,   // Animate
    VIR_KEYWORD,       // Vir
    ARROW,             // ->
    EVENT_BIND,        // &->
    EOF_JS,
};

struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string lexeme;
    int line;
};

class CHTLJSLexer {
public:
    explicit CHTLJSLexer(const std::string& source);
    std::vector<CHTLJSToken> scanTokens();
private:
    const std::string& source;
    std::vector<CHTLJSToken> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
};

} // namespace CHTL

#endif // CHTLJS_LEXER_H
