#ifndef CHTL_JS_LEXER_H
#define CHTL_JS_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL_JS {

class CHTLJSLexer {
public:
    explicit CHTLJSLexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    const std::string& source;
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    int line = 1;

    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string(char quote);
    void number();
    void identifier();
    void scanToken();
};

} // namespace CHTL_JS

#endif // CHTL_JS_LEXER_H
