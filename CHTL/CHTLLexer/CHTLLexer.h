#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>

namespace CHTL {

class CHTLLexer {
public:
    CHTLLexer(const std::string& source);

    Token getNextToken();
    std::string readRawContentUntil(char terminator);

private:
    std::string m_source;
    size_t m_position;
    int m_line;
    int m_column;

    char peek();
    char advance();
    void skipWhitespace();

    Token makeToken(TokenType type, const std::string& value);
    Token identifier();
    Token stringLiteral(char quoteType);
    Token comment();
};

} // namespace CHTL

#endif // CHTL_LEXER_H
