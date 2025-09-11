#pragma once

#include "Token.h"
#include <string>

namespace CHTL_JS {

class CHTLJSLexer {
public:
    CHTLJSLexer(const std::string& input);

    Token NextToken();

private:
    void readChar();
    char peekChar() const;

    const std::string m_input;
    size_t m_position = 0;
    size_t m_readPosition = 0;
    char m_char = 0;
};

} // namespace CHTL_JS
