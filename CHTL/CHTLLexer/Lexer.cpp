#include "Lexer.h"
#include <cctype>
#include <map>

namespace CHTL {

// Map keywords to their token types
const std::map<std::string, TokenType> keywords = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
};

Lexer::Lexer(std::string input) : m_input(std::move(input)) {
    readChar(); // Initialize m_ch, m_position, and m_readPosition
}

void Lexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_ch = 0; // EOF
    } else {
        m_ch = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition++;
}

char Lexer::peekChar() const {
    if (m_readPosition >= m_input.length()) {
        return 0; // EOF
    }
    return m_input[m_readPosition];
}

void Lexer::skipWhitespace() {
    while (m_ch == ' ' || m_ch == '\t' || m_ch == '\n' || m_ch == '\r') {
        if (m_ch == '\n') {
            m_line++;
        }
        readChar();
    }
}

bool Lexer::isIdentifierChar(char c) const {
    return std::isalnum(c) || c == '_' || c == '-';
}

Token Lexer::readIdentifier() {
    size_t start_pos = m_position;
    while (isIdentifierChar(m_ch)) {
        readChar();
    }
    std::string literal = m_input.substr(start_pos, m_position - start_pos);

    // Check if the identifier is a keyword
    if (keywords.count(literal)) {
        return {keywords.at(literal), literal, m_line};
    }

    return {TokenType::IDENTIFIER, literal, m_line};
}

Token Lexer::readQuotedString(char quote_type) {
    readChar(); // Consume the opening quote
    size_t start_pos = m_position;
    while (m_ch != quote_type && m_ch != 0) {
        readChar();
    }
    std::string literal = m_input.substr(start_pos, m_position - start_pos);
    readChar(); // Consume the closing quote
    return {TokenType::STRING, literal, m_line};
}

Token Lexer::readString() {
    // This handles unquoted strings. It reads until a character that
    // cannot be part of an unquoted value is found (e.g., ';', '}', '{').
    size_t start_pos = m_position;
    while (m_ch != ';' && m_ch != '}' && m_ch != '{' && m_ch != '\n' && m_ch != 0) {
        readChar();
    }
    std::string literal = m_input.substr(start_pos, m_position - start_pos);
    // Trim trailing whitespace from the unquoted string
    literal.erase(literal.find_last_not_of(" \t\r") + 1);
    return {TokenType::STRING, literal, m_line};
}


Token Lexer::nextToken() {
    Token tok;
    skipWhitespace();

    // Handle comments
    if (m_ch == '/') {
        if (peekChar() == '/') { // Single-line comment
            readChar(); // consume '/'
            readChar(); // consume '/'
            while (m_ch != '\n' && m_ch != 0) {
                readChar();
            }
            skipWhitespace(); // Skip whitespace after comment
            return nextToken(); // Get next token
        } else if (peekChar() == '*') { // Multi-line comment
            readChar(); // consume '/'
            readChar(); // consume '*'
            while (m_ch != 0 && (m_ch != '*' || peekChar() != '/')) {
                if (m_ch == '\n') m_line++;
                readChar();
            }
            if (m_ch != 0) readChar(); // consume '*'
            if (m_ch != 0) readChar(); // consume '/'
            skipWhitespace();
            return nextToken();
        }
    }

    tok.line = m_line;
    switch (m_ch) {
        case '{': tok = {TokenType::LEFT_BRACE, "{", m_line}; break;
        case '}': tok = {TokenType::RIGHT_BRACE, "}", m_line}; break;
        case ':': tok = {TokenType::COLON, ":", m_line}; break;
        case '=': tok = {TokenType::EQUALS, "=", m_line}; break;
        case ';': tok = {TokenType::SEMICOLON, ";", m_line}; break;
        case '.': tok = {TokenType::DOT, ".", m_line}; break;
        case '#': tok = {TokenType::HASH, "#", m_line}; break;
        case '&': tok = {TokenType::AMPERSAND, "&", m_line}; break;
        case '"': return readQuotedString('"');
        case '\'': return readQuotedString('\'');
        case 0: tok = {TokenType::END_OF_FILE, "", m_line}; break;
        default:
            if (isIdentifierChar(m_ch)) {
                return readIdentifier();
            }
            // In some contexts (like attribute values), we expect an unquoted string.
            // The parser will be responsible for deciding when to call a specific
            // readString method. For now, a simple lexer will treat many things
            // as identifiers. A more advanced version will need context from the parser.
            // Let's assume for now that if it's not a known symbol or identifier, it's illegal.
            tok = {TokenType::ILLEGAL, std::string(1, m_ch), m_line};
            break;
    }

    readChar();
    return tok;
}

} // namespace CHTL
