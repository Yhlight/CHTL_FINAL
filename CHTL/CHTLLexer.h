#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

class Lexer {
public:
    explicit Lexer(std::string source);

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

    void consumeString();
    void consumeIdentifier();
    void consumeBlockComment();
    void consumeLineComment();
    void consumeGeneratorComment();


    std::string source_;
    std::vector<Token> tokens_;
    size_t start_ = 0;
    size_t current_ = 0;
    int line_ = 1;

    static const std::map<std::string, TokenType> keywords_;
};

} // namespace CHTL

#endif // CHTL_LEXER_H
