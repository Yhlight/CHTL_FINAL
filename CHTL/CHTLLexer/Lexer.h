#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

class Lexer {
public:
    explicit Lexer(const std::string& source);

    std::vector<Token> tokenize();

private:
    Token nextToken();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    Token readStringLiteral(char quote_type);
    Token readIdentifierOrKeyword();
    Token readBracketedKeyword();
    char peek() const;
    char advance();

    const std::string m_source;
    size_t m_current = 0;
    size_t m_start = 0;
    int m_line = 1;
    int m_column = 1;

    static const std::map<std::string, TokenType> s_keywords;
    static const std::map<std::string, TokenType> s_bracketed_keywords;
    static const std::map<std::string, TokenType> s_type_prefixes;
};

} // namespace CHTL
