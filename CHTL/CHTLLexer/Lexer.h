#pragma once

#include "Token.h"
#include <string>

namespace CHTL {

class Lexer {
public:
    Lexer(std::string input);

    Token nextToken();

private:
    void readChar();
    char peekChar();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();

    Token readIdentifier();
    Token readString();
    Token readNumber();
    Token readUnquotedLiteral();

    std::string m_input;
    int m_position;      // current position in input (points to current char)
    int m_readPosition;  // current reading position in input (after current char)
    char m_char;         // current char under examination
    int m_line;
    int m_column;
};

} // namespace CHTL
