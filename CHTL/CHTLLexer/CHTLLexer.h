#pragma once

#include "Token.h"
#include <string>
#include <map>

namespace CHTL {

class CHTLLexer {
public:
    CHTLLexer(const std::string& input);

    Token NextToken();
    const std::string& GetInput() const { return m_input; }
    size_t GetPosition() const { return m_position; }
    size_t GetReadPosition() const { return m_readPosition; }

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
    TokenType lookupIdent(const std::string& ident);

    const std::string m_input;
    static const std::map<std::string, TokenType> m_keywords;
    size_t m_position = 0;
    size_t m_readPosition = 0;
    char m_char = 0;
    int m_line = 1;
    int m_col = 0;
};

} // namespace CHTL
