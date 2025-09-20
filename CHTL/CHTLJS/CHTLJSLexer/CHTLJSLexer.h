#ifndef CHTLJS_LEXER_H
#define CHTLJS_LEXER_H

#include <string>
#include <vector>

namespace CHTL_JS {

// CHTL JS will have its own set of tokens.
enum class TokenType {
    // ... to be defined ...
    ENHANCED_SELECTOR, // {{...}}
    LISTEN_KEYWORD,    // Listen
    DELEGATE_KEYWORD,  // Delegate
    ANIMATE_KEYWORD,   // Animate
    VIR_KEYWORD,       // Vir
    ARROW,             // ->
    EVENT_BIND,        // &->
    EOF_TOKEN,
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

class CHTMLJSLexer {
public:
    explicit CHTMLJSLexer(const std::string& source);
    std::vector<Token> scanTokens();
private:
    const std::string& source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
};

} // namespace CHTL

#endif // CHTLJS_LEXER_H
