#pragma once

#include "Token.h"
#include <string>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& input);
    Token nextToken();

private:
    void readChar();
    char peekChar() const;
    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();
    Token readIdentifier();
    Token readString(char quote);
    Token readUnquotedLiteral();

    std::string m_input;
    size_t m_position = 0;
    size_t m_readPosition = 0;
    char m_char = 0;
    int m_line = 1;
    int m_column = 0;
};

} // namespace CHTL
