#include "CHTLJSLexer.h"
#include <cctype>

namespace CHTL_JS {

CHTLJSLexer::CHTLJSLexer(const std::string& input) : m_input(input) {
    readChar();
}

void CHTLJSLexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_char = 0; // EOF
    } else {
        m_char = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition++;
}

char CHTLJSLexer::peekChar() const {
    if (m_readPosition >= m_input.length()) {
        return 0;
    }
    return m_input[m_readPosition];
}

Token CHTLJSLexer::NextToken() {
    size_t start_pos = m_position;

    // Scan for the next special sequence {{ or }}
    while (m_char != 0 && (m_char != '{' || peekChar() != '{') && (m_char != '}' || peekChar() != '}')) {
        readChar();
    }

    // If we found raw JS code before the special sequence, return it
    if (m_position > start_pos) {
        return {TokenType::TOKEN_JS_CODE, m_input.substr(start_pos, m_position - start_pos)};
    }

    // Handle the special sequences
    if (m_char == '{' && peekChar() == '{') {
        readChar(); readChar();
        return {TokenType::TOKEN_LDBRACE, "{{"};
    }
    if (m_char == '}' && peekChar() == '}') {
        readChar(); readChar();
        return {TokenType::TOKEN_RDBRACE, "}}"};
    }

    if (m_char == 0) {
        return {TokenType::TOKEN_EOF, ""};
    }

    // Should not be reached if logic is correct
    readChar();
    return {TokenType::TOKEN_ILLEGAL, m_input.substr(m_position - 1, 1)};
}


} // namespace CHTL_JS
