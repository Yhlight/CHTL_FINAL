#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include "GlobalMap.h"
#include <string>
#include <vector>

namespace CHTL {

class CHTLLexer {
public:
    CHTLLexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    bool isAtEnd() const;
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);

    char peek() const;

    void handleIdentifier();

    const std::string& source;
    std::vector<Token> tokens;
    GlobalMap globalMap;

    int start = 0;
    int current = 0;
    int line = 1;
    int column = 1;
    int start_column = 1;
};

} // namespace CHTL

#endif // CHTL_LEXER_H
