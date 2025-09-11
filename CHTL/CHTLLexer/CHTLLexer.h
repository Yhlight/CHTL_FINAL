#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class CHTLLexer {
public:
    explicit CHTLLexer(std::string source);
    Token GetNextToken();
    std::vector<Token> Tokenize();

private:
    std::string m_source;
    size_t m_cursor = 0;
    size_t m_line = 1;
    size_t m_column = 1;

    char Peek(size_t offset = 0);
    char Consume();
    void ConsumeWhitespace();

    Token CreateToken(TokenType type, std::string value, size_t pos);
    Token HandleIdentifier();
    Token HandleStringLiteral();
    Token HandleNumber();
};

} // namespace CHTL
