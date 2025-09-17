#include "Lexer.h"
#include <cctype>
#include <map>

namespace CHTL {

// Map keywords to their token types
static const std::map<std::string, TokenType> keywords = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"inherit", TokenType::INHERIT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"except", TokenType::EXCEPT},
    {"use", TokenType::USE},
    {"html5", TokenType::HTML5},
};

Lexer::Lexer(std::string input)
    : m_input(std::move(input)), m_position(0), m_readPosition(0), m_char(0), m_line(1), m_column(0) {
    readChar();
}

void Lexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_char = 0; // EOF
    } else {
        m_char = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition++;
    m_column++;
}

char Lexer::peekChar() {
    if (m_readPosition >= m_input.length()) {
        return 0;
    }
    return m_input[m_readPosition];
}

void Lexer::skipWhitespace() {
    while (m_char == ' ' || m_char == '\t' || m_char == '\n' || m_char == '\r') {
        if (m_char == '\n') {
            m_line++;
            m_column = 0;
        }
        readChar();
    }
}

void Lexer::skipLineComment() {
    while (m_char != '\n' && m_char != 0) {
        readChar();
    }
    skipWhitespace();
}

void Lexer::skipBlockComment() {
    readChar(); // Consume '*'
    while (m_char != 0) {
        if (m_char == '*' && peekChar() == '/') {
            readChar();
            readChar();
            break;
        }
        if (m_char == '\n') {
            m_line++;
            m_column = 0;
        }
        readChar();
    }
    skipWhitespace();
}


Token Lexer::readIdentifier() {
    int start_pos = m_position;
    while (std::isalpha(m_char) || m_char == '_') {
        readChar();
    }
    std::string literal = m_input.substr(start_pos, m_position - start_pos);

    auto it = keywords.find(literal);
    if (it != keywords.end()) {
        return {it->second, literal, m_line, m_column - (int)literal.length()};
    }

    return {TokenType::IDENTIFIER, literal, m_line, m_column - (int)literal.length()};
}

Token Lexer::readString() {
    char quote_type = m_char;
    int start_pos = m_position + 1;
    readChar();
    while (m_char != quote_type && m_char != 0) {
        if (m_char == '\\') {
            readChar();
        }
        readChar();
    }
    std::string literal = m_input.substr(start_pos, m_position - start_pos);
    readChar(); // Consume closing quote
    return {TokenType::STRING_LITERAL, literal, m_line, m_column - (int)literal.length() - 2};
}

Token Lexer::readNumber() {
    int start_pos = m_position;
    while (std::isdigit(m_char) || m_char == '.') {
        readChar();
    }
    // After reading the number part, check for an alphabetic unit (e.g., px, em)
    while (std::isalpha(m_char)) {
        readChar();
    }
    std::string literal = m_input.substr(start_pos, m_position - start_pos);
    // Let's classify this as UNQUOTED_LITERAL for simplicity, as it's not a pure number.
    return {TokenType::UNQUOTED_LITERAL, literal, m_line, m_column - (int)literal.length()};
}


Token Lexer::nextToken() {
    skipWhitespace();

    Token tok;
    tok.line = m_line;
    tok.column = m_column;

    switch (m_char) {
        case '=': tok = {TokenType::ASSIGN, "=", m_line, m_column}; break;
        case ':': tok = {TokenType::COLON, ":", m_line, m_column}; break;
        case ';': tok = {TokenType::SEMICOLON, ";", m_line, m_column}; break;
        case '(': tok = {TokenType::LPAREN, "(", m_line, m_column}; break;
        case ')': tok = {TokenType::RPAREN, ")", m_line, m_column}; break;
        case '{': tok = {TokenType::LBRACE, "{", m_line, m_column}; break;
        case '}': tok = {TokenType::RBRACE, "}", m_line, m_column}; break;
        case '[': tok = {TokenType::LBRACKET, "[", m_line, m_column}; break;
        case ']': tok = {TokenType::RBRACKET, "]", m_line, m_column}; break;
        case ',': tok = {TokenType::COMMA, ",", m_line, m_column}; break;
        case '+': tok = {TokenType::PLUS, "+", m_line, m_column}; break;
        case '-':
            if (peekChar() == '>') { readChar(); tok = {TokenType::ARROW, "->", m_line, m_column - 1}; }
            else { tok = {TokenType::MINUS, "-", m_line, m_column}; }
            break;
        case '/':
            if (peekChar() == '/') { skipLineComment(); return nextToken(); }
            else if (peekChar() == '*') { skipBlockComment(); return nextToken(); }
            else { tok = {TokenType::SLASH, "/", m_line, m_column}; }
            break;
        case '*':
             if (peekChar() == '*') { readChar(); tok = {TokenType::POWER, "**", m_line, m_column - 1}; }
             else { tok = {TokenType::ASTERISK, "*", m_line, m_column}; }
            break;
        case '%': tok = {TokenType::PERCENT, "%", m_line, m_column}; break;
        case '&':
            if (peekChar() == '&') { readChar(); tok = {TokenType::AND, "&&", m_line, m_column - 1}; }
            else { tok = {TokenType::AMPERSAND, "&", m_line, m_column}; }
            break;
        case '|':
             if (peekChar() == '|') { readChar(); tok = {TokenType::OR, "||", m_line, m_column - 1}; }
             else { tok = {TokenType::ILLEGAL, std::string(1, m_char), m_line, m_column}; }
            break;
        case '>': tok = {TokenType::GT, ">", m_line, m_column}; break;
        case '<': tok = {TokenType::LT, "<", m_line, m_column}; break;
        case '"':
        case '\'':
            return readString();
        case '#':
            skipLineComment();
            return nextToken();
        case 0:
            tok = {TokenType::END_OF_FILE, "", m_line, m_column};
            break;
        default:
            if (std::isalpha(m_char) || m_char == '_') {
                return readIdentifier();
            } else if (std::isdigit(m_char)) {
                // Now correctly handles dimensions like "100px"
                return readNumber();
            } else {
                tok = {TokenType::ILLEGAL, std::string(1, m_char), m_line, m_column};
            }
            break;
    }

    readChar();
    return tok;
}

} // namespace CHTL
