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
    // An identifier starts with a letter or _, followed by letters, numbers, or _.
    while (std::isalnum(m_char) || m_char == '_') {
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
    while (std::isalpha(m_char)) {
        readChar();
    }
    std::string literal = m_input.substr(start_pos, m_position - start_pos);
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
        case '[':
            {
                readChar(); // consume '['
                Token id = readIdentifier();
                if (m_char != ']') {
                    tok = {TokenType::ILLEGAL, "[" + id.literal, m_line, m_column};
                } else {
                    if (id.literal == "Template") tok = {TokenType::TEMPLATE, "[Template]", m_line, m_column - 10};
                    else if (id.literal == "Custom") tok = {TokenType::CUSTOM, "[Custom]", m_line, m_column - 8};
                    else if (id.literal == "Origin") tok = {TokenType::ORIGIN, "[Origin]", m_line, m_column - 8};
                    else if (id.literal == "Import") tok = {TokenType::IMPORT, "[Import]", m_line, m_column - 8};
                    else if (id.literal == "Namespace") tok = {TokenType::NAMESPACE, "[Namespace]", m_line, m_column - 11};
                    else if (id.literal == "Configuration") tok = {TokenType::CONFIGURATION, "[Configuration]", m_line, m_column - 15};
                    else tok = {TokenType::ILLEGAL, "[" + id.literal + "]", m_line, m_column};
                }
                readChar();
                return tok;
            }
        case ']': tok = {TokenType::RBRACKET, "]", m_line, m_column}; break;
        case ',': tok = {TokenType::COMMA, ",", m_line, m_column}; break;
        case '@':
            {
                readChar();
                Token id = readIdentifier();
                if (id.literal == "Style") tok = {TokenType::AT_STYLE, "@Style", m_line, m_column - 6};
                else if (id.literal == "Element") tok = {TokenType::AT_ELEMENT, "@Element", m_line, m_column - 8};
                else if (id.literal == "Var") tok = {TokenType::AT_VAR, "@Var", m_line, m_column - 4};
                else tok = {TokenType::ILLEGAL, "@" + id.literal, m_line, m_column};
                return tok;
            }
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
