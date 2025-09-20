#ifndef CHTLJS_LEXER_H
#define CHTLJS_LEXER_H

#include "CHTLJSToken.h" // Use the separate token header
#include <string>
#include <vector>
#include <map>

namespace CHTLJS {

class CHTLJSLexer {
public:
    explicit CHTLJSLexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void identifier();

    const std::string& source;
    std::vector<Token> tokens;
    static const std::map<std::string, TokenType> keywords;

    int start = 0;
    int current = 0;
    int line = 1;
};

} // namespace CHTLJS

#endif // CHTLJS_LEXER_H
