#include "CHTLLexer.h"
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& source)
    : m_source(source), m_position(0), m_line(1), m_column(1) {}

char CHTLLexer::peek() {
    if (m_position >= m_source.length()) {
        return '\0';
    }
    return m_source[m_position];
}

char CHTLLexer::advance() {
    if (m_position < m_source.length()) {
        char current = m_source[m_position];
        m_position++;
        if (current == '\n') {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        return current;
    }
    return '\0'; // End of file
}

void CHTLLexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

Token CHTLLexer::makeToken(TokenType type, const std::string& value) {
    return {type, value, m_line, m_column};
}

Token CHTLLexer::identifier() {
    std::string value;
    size_t start_pos = m_position;
    // CHTL identifiers can contain hyphens, e.g., in CSS properties.
    while (isalnum(peek()) || peek() == '_' || peek() == '-') {
        value += advance();
    }
    return makeToken(TokenType::IDENTIFIER, value);
}

Token CHTLLexer::stringLiteral(char quoteType) {
    std::string value;
    advance(); // consume opening quote
    while (peek() != quoteType && peek() != '\0') {
        value += advance();
    }
    if (peek() == '\0') {
        // Unterminated string
        return makeToken(TokenType::UNKNOWN, value);
    }
    advance(); // consume closing quote
    return makeToken(TokenType::STRING_LITERAL, value);
}

Token CHTLLexer::comment() {
    char p = peek();
    if (p == '/') {
        advance(); // consume first /
        if (peek() == '/') { // Single line comment
            advance(); // consume second /
            std::string value;
            while (peek() != '\n' && peek() != '\0') {
                value += advance();
            }
            return makeToken(TokenType::SINGLE_LINE_COMMENT, value);
        } else if (peek() == '*') { // Multi-line comment
            advance(); // consume *
            std::string value;
            while (!(peek() == '*' && m_position + 1 < m_source.length() && m_source[m_position + 1] == '/') && peek() != '\0') {
                value += advance();
            }
            if (peek() != '\0') {
                advance(); // consume *
                advance(); // consume /
            }
            return makeToken(TokenType::MULTI_LINE_COMMENT, value);
        }
        // This case should be handled carefully, maybe it's just a division operator in an expression.
        // For now, CHTL doesn't seem to have division, so we can assume it's an error or part of an unquoted literal.
        return makeToken(TokenType::UNKNOWN, "/");
    } else if (p == '-') {
        advance(); // consume first -
        if (peek() == '-') { // Generator comment
            advance(); // consume second -
            std::string value;
            while (peek() != '\n' && peek() != '\0') {
                value += advance();
            }
            return makeToken(TokenType::GENERATOR_COMMENT, value);
        }
        // If it's just one dash, it's part of an identifier or unquoted literal.
        // Let's backtrack and let the identifier handler take it.
        m_position--;
        m_column--;
        return identifier();
    }
    return makeToken(TokenType::UNKNOWN, "");
}

std::string CHTLLexer::readRawContentUntil(char terminator) {
    std::string content;
    while(peek() != terminator && peek() != '\0') {
        content += advance();
    }
    return content;
}

Token CHTLLexer::getNextToken() {
    skipWhitespace();

    char current = peek();

    if (current == '\0') {
        return makeToken(TokenType::END_OF_FILE, "");
    }

    if (isalpha(current) || current == '_') {
        return identifier();
    }

    if (current == '"' || current == '\'') {
        return stringLiteral(current);
    }

    // Check for comments before checking for single characters like '-'
    if (current == '/' && m_position + 1 < m_source.length()) {
        char next = m_source[m_position + 1];
        if (next == '/' || next == '*') {
            return comment();
        }
    }
    if (current == '-' && m_position + 1 < m_source.length() && m_source[m_position + 1] == '-') {
        return comment();
    }

    // Single character tokens
    switch (current) {
        case '{': advance(); return makeToken(TokenType::L_BRACE, "{");
        case '}': advance(); return makeToken(TokenType::R_BRACE, "}");
        case '(': advance(); return makeToken(TokenType::L_PAREN, "(");
        case ')': advance(); return makeToken(TokenType::R_PAREN, ")");
        case '[': advance(); return makeToken(TokenType::L_BRACKET, "[");
        case ']': advance(); return makeToken(TokenType::R_BRACKET, "]");
        case ':': advance(); return makeToken(TokenType::COLON, ":");
        case '=': advance(); return makeToken(TokenType::EQUALS, "=");
        case ';': advance(); return makeToken(TokenType::SEMICOLON, ";");
        case ',': advance(); return makeToken(TokenType::COMMA, ",");
        case '@': advance(); return makeToken(TokenType::AT, "@");
        case '#': advance(); return makeToken(TokenType::HASH, "#");
        case '.': advance(); return makeToken(TokenType::DOT, ".");
        case '&': advance(); return makeToken(TokenType::AMPERSAND, "&");
        case '?': advance(); return makeToken(TokenType::QUESTION, "?");
    }

    // If we are here, it could be a number, a lone symbol, or part of an unquoted literal.
    // Let's treat numbers and other valid identifier characters as identifiers.
    // The spec says unquoted literals are like CSS, which can start with numbers (e.g., `font-weight: 600`).
    if (isdigit(current) || current == '-') {
        return identifier();
    }

    // If we still haven't recognized the token, it's unknown.
    return makeToken(TokenType::UNKNOWN, std::string(1, advance()));
}

} // namespace CHTL
