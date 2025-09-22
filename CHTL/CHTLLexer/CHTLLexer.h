#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../Config/Configuration.h"

namespace CHTL {

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& source, std::shared_ptr<Configuration> config);
    std::vector<Token> scanTokens();

private:
    std::shared_ptr<Configuration> config;
    std::unordered_map<std::string, TokenType> runtime_keyword_map;
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
    void scanReactiveVar();

    const std::string& source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
};

} // namespace CHTL

#endif // CHTL_LEXER_H
