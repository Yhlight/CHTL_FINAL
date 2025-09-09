#pragma once

#include "Token.h"
#include <string>
#include <unordered_map>

namespace CHTL {

class Lexer {
public:
    Lexer(std::string input);

    Token nextToken();

private:
    void readChar();
    char peekChar() const;
    std::string readIdentifier();
    std::string readStringLiteral(char quote_type);
    std::string readNumberLiteral();
    void skipWhitespace();
    void skipLineComment();
    std::string readBlockComment();

    TokenType lookupIdent(const std::string& ident);

    std::string m_input;
    size_t m_position;      // current position in input (points to current char)
    size_t m_readPosition;  // current reading position in input (after current char)
    char m_ch;              // current char under examination
    int m_currentLine;
};

} // namespace CHTL
