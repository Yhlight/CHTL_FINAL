#pragma once

#include "Token.h"
#include <string>

namespace CHTL {

class CHTLLexer {
public:
    CHTLLexer(const std::string& input);

    Token NextToken();

private:
    void readChar();
    char peekChar() const;
    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();
    std::string readIdentifier();
    std::string readNumber();
    std::string readString(char quote);
    std::string readUnquotedLiteral();

    const std::string m_input;
    size_t m_position = 0;      // current position in input (points to current char)
    size_t m_readPosition = 0;  // current reading position in input (after current char)
    char m_char = 0;            // current char under examination
    int m_line = 1;
    int m_col = 0;
};

} // namespace CHTL
