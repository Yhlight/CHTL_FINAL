#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include "Token.h"

namespace CHTL {

class CHTLLexer {
public:
    CHTLLexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    const std::string& source;
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    int line = 1;

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string(char quote);
    void number();
    void identifier();
    void skipBlockComment();
};

} // namespace CHTL

#endif // CHTL_LEXER_H
