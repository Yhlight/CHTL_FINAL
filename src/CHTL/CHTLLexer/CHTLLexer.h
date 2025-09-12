#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& source);

    std::vector<Token> scanTokens();

private:
    const std::string& source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd();
    void scanToken();
    char advance();
    char peek();
    char peekNext();

    void addToken(TokenType type);
    // Overload for tokens with a different literal value than their lexeme (e.g., string content)
    void addToken(TokenType type, const std::string& literal);

    void stringLiteral();
    void numberLiteral();
    void identifier();
};

} // namespace CHTL
