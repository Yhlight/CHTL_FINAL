#ifndef CHTL_JS_LEXER_H
#define CHTL_JS_LEXER_H

#include "CHTLJSToken.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {
namespace CHTLJS {

class CHTLJSLexer {
public:
    explicit CHTLJSLexer(const std::string& source);
    std::vector<CHTLJSToken> scanTokens();

private:
    void scanToken();
    void identifier();
    void number();
    void stringLiteral(char quote);

    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;
    void addToken(CHTLJSTokenType type);

    const std::string& source_;
    std::vector<CHTLJSToken> tokens_;
    size_t start_ = 0;
    size_t current_ = 0;
    size_t line_ = 1;

    static const std::unordered_map<std::string, CHTLJSTokenType> keywords;
};

} // namespace CHTLJS
} // namespace CHTL

#endif // CHTL_JS_LEXER_H
