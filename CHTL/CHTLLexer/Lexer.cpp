#include "Lexer.h"
#include <cctype>

namespace CHTL {

Lexer::Lexer(const std::string& input)
    : m_input(input), m_position(0), m_readPosition(0), m_char(0), m_line(1), m_column(0) {
    readChar(); // Initialize the first character
}

void Lexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_char = 0; // 0 is ASCII code for "NUL" character, signifies EOF
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

Token Lexer::readIdentifier() {
    int start_pos = m_position;
    // Assumes the first character is already checked to be a letter or underscore
    while (isalnum(m_char) || m_char == '_' || m_char == '-') {
        readChar();
    }
    return {TokenType::IDENTIFIER, m_input.substr(start_pos, m_position - start_pos), m_line, m_column};
}

Token Lexer::readString() {
    int start_pos = m_position + 1; // After the opening quote
    readChar(); // consume opening '"'
    while (m_char != '"' && m_char != 0) {
        readChar();
    }
    std::string literal = m_input.substr(start_pos, m_position - start_pos);
    readChar(); // consume closing '"'
    return {TokenType::STRING, literal, m_line, m_column};
}

Token Lexer::NextToken() {
    // A better implementation for skipping
    while (m_char == ' ' || m_char == '\t' || m_char == '\n' || m_char == '\r' || m_char == '/') {
        if (m_char == ' ' || m_char == '\t' || m_char == '\r') {
            readChar();
            continue;
        }
        if (m_char == '\n') {
            m_line++;
            m_column = 0;
            readChar();
            continue;
        }
        if (m_char == '/') {
            if (peekChar() == '/') {
                // Skip single-line comment
                while (m_char != '\n' && m_char != 0) {
                    readChar();
                }
                continue;
            } else if (peekChar() == '*') {
                readChar(); // consume '/'
                readChar(); // consume '*'
                while (m_char != 0 && !(m_char == '*' && peekChar() == '/')) {
                     if (m_char == '\n') {
                        m_line++;
                        m_column = 0;
                    }
                    readChar();
                }
                if (m_char != 0) readChar(); // consume '*'
                if (m_char != 0) readChar(); // consume '/'
                continue;
            } else {
                break; // it's a division operator, not a comment
            }
        }
    }


    Token tok;
    tok.line = m_line;
    tok.column = m_column;

    switch (m_char) {
        case '{':
            tok = {TokenType::LBRACE, "{", m_line, m_column};
            break;
        case '}':
            tok = {TokenType::RBRACE, "}", m_line, m_column};
            break;
        case ':':
            tok = {TokenType::COLON, ":", m_line, m_column};
            break;
        case '=':
            tok = {TokenType::ASSIGN, "=", m_line, m_column};
            break;
        case ';':
            tok = {TokenType::SEMICOLON, ";", m_line, m_column};
            break;
        case '"':
            return readString();
        case 0:
            tok = {TokenType::END_OF_FILE, "", m_line, m_column};
            break;
        default:
            if (isalpha(m_char) || m_char == '_') {
                // Don't readChar() here, readIdentifier will do it
                return readIdentifier();
            } else {
                tok = {TokenType::ILLEGAL, std::string(1, m_char), m_line, m_column};
            }
            break;
    }

    readChar();
    return tok;
}

} // namespace CHTL
