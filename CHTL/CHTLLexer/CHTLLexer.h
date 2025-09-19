#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    bool isAtEnd();
    char advance();
    bool match(char expected);
    char peek();
    char peekNext();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);
    void string(char quote);
    void number();
    void identifier();
    void scanBlockKeyword();

    const std::string& source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
};

} // namespace CHTL

#endif // CHTL_LEXER_H
