#pragma once

#include "Token.h"
#include <string>
#include <unordered_map>

namespace CHTL {

class CHTLLexer {
public:
    CHTLLexer(std::string input);
    Token NextToken();

private:
    void readChar();
    char peekChar() const;
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();

    Token readIdentifierOrKeyword();
    Token readString(char quoteType);
    Token readNumber();
    Token readHashComment();
    Token readBracketedKeyword();

    std::string m_input;
    size_t m_position;     // current position in input (points to current char)
    size_t m_readPosition; // current reading position in input (after current char)
    char m_char;           // current char under examination
    int m_line;
    int m_column;

    static const std::unordered_map<std::string, TokenType> s_keywords;
};

} // namespace CHTL
