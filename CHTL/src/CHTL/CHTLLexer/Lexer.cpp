#include "Lexer.h"
#include <cctype>

namespace CHTL {

Lexer::Lexer(const std::string& source) : m_source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::EndOfFile);
    return tokens;
}

Token Lexer::nextToken() {
    m_start = m_current;

    if (m_current >= m_source.length()) {
        return makeToken(TokenType::EndOfFile);
    }

    char c = advance();

    switch (c) {
        // Whitespace
        case ' ':
        case '\r':
        case '\t':
            return nextToken(); // Skip whitespace and get the next token
        case '\n':
            m_line++;
            return nextToken(); // Skip newline and get the next token

        // Single-character tokens
        case '{': return makeToken(TokenType::OpenBrace, "{");
        case '}': return makeToken(TokenType::CloseBrace, "}");
        case ':': return makeToken(TokenType::Colon, ":");
        case '=': return makeToken(TokenType::Equals, "=");
        case ';': return makeToken(TokenType::Semicolon, ";");

        // String literals
        case '"':
        case '\'':
            return makeString(c);

        // Comments or division
        case '/':
            if (peek() == '/') {
                skipLineComment();
                return nextToken(); // Rerun to get the next valid token
            }
            if (peek() == '*') {
                skipBlockComment();
                return nextToken(); // Rerun to get the next valid token
            }
            // If not a comment, it's part of an unquoted literal, fall through.
            break;

        // Generator comments
        case '-':
            if (peek() == '-') {
                skipGeneratorComment();
                return nextToken(); // Rerun to get the next valid token
            }
            // If not a generator comment, it's part of an unquoted literal, fall through.
            break;
    }

    // If we fall through, it's an identifier or unquoted literal.
    // Backtrack one character since we consumed it in the switch.
    m_current--;
    return makeIdentifierOrUnquotedLiteral();
}

Token Lexer::makeIdentifierOrUnquotedLiteral() {
    m_start = m_current;

    // Consume characters until a delimiter is found.
    while (m_current < m_source.length()) {
        char c = peek();
        if (std::isspace(c) || c == '{' || c == '}' || c == ':' || c == ';' || c == '=') {
            break;
        }
        // Check for comment starts as delimiters
        if (c == '/' && m_current + 1 < m_source.length() && (m_source[m_current+1] == '/' || m_source[m_current+1] == '*')) {
            break;
        }
        if (c == '-' && m_current + 1 < m_source.length() && m_source[m_current+1] == '-') {
            break;
        }
        advance();
    }

    std::string value = m_source.substr(m_start, m_current - m_start);

    // According to CHTL spec, element names and attributes are identifiers.
    // Values can be unquoted literals. For now, we will classify anything
    // that is not a keyword as Identifier, as the parser will determine its role.
    // This simplifies the lexer.
    return makeToken(TokenType::Identifier, value);
}

Token Lexer::makeString(char quote_type) {
    m_start = m_current; // Start after the opening quote
    while (peek() != quote_type && m_current < m_source.length()) {
        if (peek() == '\n') m_line++;
        advance();
    }

    if (m_current >= m_source.length()) {
        return makeToken(TokenType::Unexpected, "Unterminated string");
    }

    std::string value = m_source.substr(m_start, m_current - m_start);
    advance(); // Consume the closing quote
    return makeToken(TokenType::StringLiteral, value);
}

void Lexer::skipLineComment() {
    while (peek() != '\n' && m_current < m_source.length()) {
        advance();
    }
}

void Lexer::skipBlockComment() {
    advance(); // Consume the '*'
    while (m_current < m_source.length() - 1) {
        if (peek() == '*' && m_source[m_current + 1] == '/') {
            advance(); // consume '*'
            advance(); // consume '/'
            return;
        }
        if (peek() == '\n') m_line++;
        advance();
    }
    // Note: No error handling for unterminated block comments for now.
}

void Lexer::skipGeneratorComment() {
    advance(); // consume the second '-'
    while (peek() != '\n' && m_current < m_source.length()) {
        advance();
    }
}

char Lexer::peek() const {
    if (m_current >= m_source.length()) return '\0';
    return m_source[m_current];
}

char Lexer::advance() {
    return m_source[m_current++];
}

Token Lexer::makeToken(TokenType type, const std::string& value) const {
    // The column would be more accurate if we tracked it properly.
    // This is an approximation.
    return {type, value, m_line, 0};
}

} // namespace CHTL
