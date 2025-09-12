#include "Lexer.h"
#include <cctype>

namespace CHTL {

Lexer::Lexer(const std::string& input) : m_input(input) {
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
    if (m_char == '\n') {
        m_line++;
        m_column = 0;
    }
}

char Lexer::peekChar() const {
    if (m_readPosition >= m_input.length()) {
        return 0;
    }
    return m_input[m_readPosition];
}

void Lexer::skipWhitespace() {
    while (m_char == ' ' || m_char == '\t' || m_char == '\n' || m_char == '\r') {
        readChar();
    }
}

void Lexer::skipSingleLineComment() {
    while (m_char != '\n' && m_char != 0) {
        readChar();
    }
}

void Lexer::skipMultiLineComment() {
    while (true) {
        if (m_char == 0) {
            // Unterminated multi-line comment
            break;
        }
        if (m_char == '*' && peekChar() == '/') {
            readChar(); // consume '*'
            readChar(); // consume '/'
            break;
        }
        readChar();
    }
}

Token Lexer::readIdentifier() {
    int start_pos = m_position;
    while (isalpha(m_char) || m_char == '_') {
        readChar();
    }
    return {TokenType::IDENTIFIER, m_input.substr(start_pos, m_position - start_pos), m_line, m_column};
}

Token Lexer::readString(char quote) {
    int start_pos = m_position + 1;
    readChar(); // consume opening quote
    while (m_char != quote && m_char != 0) {
        readChar();
    }
    std::string literal = m_input.substr(start_pos, m_position - start_pos);
    readChar(); // consume closing quote
    return {TokenType::STRING, literal, m_line, m_column};
}

Token Lexer::readUnquotedLiteral() {
    int start_pos = m_position;
    // Unquoted literals can contain almost anything until a delimiter.
    // This is a simplified rule for now. The parser will provide more context.
    while (m_char != ';' && m_char != '{' && m_char != '}' && m_char != '\n' && m_char != 0) {
        readChar();
    }
    // Trim trailing whitespace from the literal
    size_t end_pos = m_position;
    while (end_pos > start_pos && isspace(m_input[end_pos - 1])) {
        end_pos--;
    }
    return {TokenType::STRING, m_input.substr(start_pos, end_pos - start_pos), m_line, m_column};
}

Token Lexer::nextToken() {
    skipWhitespace();

    Token tok;
    tok.line = m_line;
    tok.column = m_column;

    switch (m_char) {
        case '{': tok = {TokenType::LBRACE, "{", m_line, m_column}; break;
        case '}': tok = {TokenType::RBRACE, "}", m_line, m_column}; break;
        case '(': tok = {TokenType::LPAREN, "(", m_line, m_column}; break;
        case ')': tok = {TokenType::RPAREN, ")", m_line, m_column}; break;
        case '[': tok = {TokenType::LBRACKET, "[", m_line, m_column}; break;
        case ']': tok = {TokenType::RBRACKET, "]", m_line, m_column}; break;
        case ':': tok = {TokenType::COLON, ":", m_line, m_column}; break;
        case '=': tok = {TokenType::EQUALS, "=", m_line, m_column}; break;
        case ';': tok = {TokenType::SEMICOLON, ";", m_line, m_column}; break;
        case ',': tok = {TokenType::COMMA, ",", m_line, m_column}; break;
        case '.': tok = {TokenType::DOT, ".", m_line, m_column}; break;
        case '&': tok = {TokenType::AMPERSAND, "&", m_line, m_column}; break;
        case '?': tok = {TokenType::QUESTION, "?", m_line, m_column}; break;
        case '+': tok = {TokenType::PLUS, "+", m_line, m_column}; break;
        case '*': tok = {TokenType::STAR, "*", m_line, m_column}; break;
        case '%': tok = {TokenType::PERCENT, "%", m_line, m_column}; break;
        case '"': return readString('"');
        case '\'': return readString('\'');
        case '-':
            if (peekChar() == '-') {
                readChar(); // consume first '-'
                readChar(); // consume second '-'
                int start_pos = m_position;
                skipSingleLineComment();
                tok = {TokenType::COMMENT_GEN, m_input.substr(start_pos, m_position - start_pos), m_line, m_column};
                // Don't call readChar() at the end for this one
                return tok;
            } else {
                tok = {TokenType::MINUS, "-", m_line, m_column};
            }
            break;
        case '/':
            if (peekChar() == '/') {
                skipSingleLineComment();
                return nextToken(); // Get next token after comment
            } else if (peekChar() == '*') {
                readChar(); // consume '/'
                readChar(); // consume '*'
                skipMultiLineComment();
                return nextToken(); // Get next token after comment
            } else {
                tok = {TokenType::SLASH, "/", m_line, m_column};
            }
            break;
        case 0:
            tok = {TokenType::END_OF_FILE, "", m_line, m_column};
            break;
        default:
            if (isalpha(m_char) || m_char == '_') {
                // This is a simplification. Keywords vs. identifiers vs. unquoted strings
                // will need more robust handling. For now, we assume alpha-starting things are identifiers.
                return readIdentifier();
            } else {
                // If it's not a recognized symbol or an identifier, it could be an unquoted literal.
                // This is a basic implementation. We'll refine this.
                if (isprint(m_char)) {
                     return readUnquotedLiteral();
                }
                tok = {TokenType::ILLEGAL, std::string(1, m_char), m_line, m_column};
            }
            break;
    }

    readChar();
    return tok;
}

} // namespace CHTL
