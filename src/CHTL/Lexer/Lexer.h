#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> getAllTokens();

private:
    Token getNextToken();
    void advance();
    char peek();
    char peekNext();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    Token identifier();
    Token stringLiteral(char quoteType);
    Token unquotedLiteral();
    Token makeToken(TokenType type, const std::string& value);

    std::string m_source;
    size_t m_current = 0;
    int m_line = 1;
    int m_column = 1;

    static const std::unordered_map<std::string, TokenType> s_keywords;
};

} // namespace CHTL

#endif // CHTL_LEXER_H
