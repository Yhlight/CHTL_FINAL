#include "Lexer.hpp"
#include <stdexcept>
#include <cstring> // For strchr
#include <iostream> // For debugging

namespace CHTL {

Lexer::Lexer(const std::string& source) : m_source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (m_current < m_source.length()) {
        m_start = m_current;
        scanToken();
    }

    m_tokens.push_back({TokenType::END_OF_FILE, "", m_line});
    return m_tokens;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '[':
            // This is a bit of a hack. A better lexer would handle all bracketed keywords.
            if (m_source.substr(m_current, 8) == "Template]") {
                std::cout << "DEBUG: Found [Template]" << std::endl;
                m_current += 8;
                addToken(TokenType::TEMPLATE);
            } else {
                std::cout << "DEBUG: Found [" << std::endl;
                addToken(TokenType::LEFT_BRACKET);
            }
            break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case '@': addToken(TokenType::AT); break;
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ':': addToken(TokenType::COLON); break;
        case '=': addToken(TokenType::EQUAL); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '+': addToken(TokenType::PLUS); break;
        case '%': addToken(TokenType::PERCENT); break;
        case '*':
            addToken(match('*') ? TokenType::STAR_STAR : TokenType::STAR);
            break;
        case '/':
            if (match('/')) {
                // A line comment goes until the end of the line.
                while (peek() != '\n' && m_current < m_source.length()) advance();
                // We don't add a token for comments, just consume them.
            } else if (match('*')) {
                // A block comment goes until */
                while (peek() != '*' || peekNext() != '/') {
                    if (peek() == '\n') m_line++;
                    if (m_current >= m_source.length()) {
                        throw std::runtime_error("Unterminated block comment.");
                    }
                    advance();
                }
                advance(); // consume *
                advance(); // consume /
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        case '-':
            if (match('-')) {
                 while (peek() != '\n' && m_current < m_source.length()) advance();
                 addToken(TokenType::GENERATOR_COMMENT);
            } else {
                addToken(TokenType::MINUS);
            }
            break;

        // Ignore whitespace.
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            m_line++;
            break;

        case '"':
        case '\'':
            string(c);
            break;

        default:
            if (isalpha(c) || c == '_') {
                identifier();
            } else if (isdigit(c)) {
                // Let's treat numbers as part of an identifier for now (e.g. 100px)
                // This is a simplification and might need refinement for pure numbers.
                identifier();
            } else {
                // For now, we'll treat other characters as part of an unquoted literal,
                // which we'll tokenize as an identifier. This is still greedy but lets us
                // handle things like `Hello CHTL!` for now. The parser will need to be smart.
                // A better solution is needed long-term.
                while (m_current < m_source.length() &&
                       !strchr("{}:; \t\n\r", peek())) {
                    advance();
                }
                std::string text = m_source.substr(m_start, m_current - m_start);
                if (!text.empty()) {
                    m_tokens.push_back({TokenType::IDENTIFIER, text, m_line});
                }
            }
            break;
    }
}

char Lexer::advance() {
    return m_source[m_current++];
}

void Lexer::addToken(TokenType type) {
    std::string text = m_source.substr(m_start, m_current - m_start);
    m_tokens.push_back({type, text, m_line});
}

bool Lexer::match(char expected) {
    if (m_current >= m_source.length()) return false;
    if (m_source[m_current] != expected) return false;

    m_current++;
    return true;
}

char Lexer::peek() {
    if (m_current >= m_source.length()) return '\0';
    return m_source[m_current];
}

char Lexer::peekNext() {
    if (m_current + 1 >= m_source.length()) return '\0';
    return m_source[m_current + 1];
}

void Lexer::string(char quote) {
    while (peek() != quote && m_current < m_source.length()) {
        if (peek() == '\n') m_line++;
        advance();
    }

    if (m_current >= m_source.length()) {
        throw std::runtime_error("Unterminated string.");
    }

    // The closing quote.
    advance();

    // Trim the surrounding quotes.
    std::string value = m_source.substr(m_start + 1, m_current - m_start - 2);
    m_tokens.push_back({TokenType::STRING, value, m_line});
}

void Lexer::identifier() {
    // An identifier is a sequence of alphanumeric characters and underscores.
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = m_source.substr(m_start, m_current - m_start);
    m_tokens.push_back({TokenType::IDENTIFIER, text, m_line});
}

} // namespace CHTL
