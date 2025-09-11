#include "CHTLLexer.h"
#include <cctype>

namespace CHTL {

// Helper function to check if a character can be part of an identifier
bool isIdentifierChar(char ch) {
    return std::isalnum(ch) || ch == '_' || ch == '-';
}

CHTLLexer::CHTLLexer(const std::string& input) : m_input(input) {
    readChar(); // Initialize the first character
}

void CHTLLexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_char = 0; // NUL character for EOF
    } else {
        m_char = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition++;
    m_col++;
    if (m_char == '\n') {
        m_line++;
        m_col = 0;
    }
}

char CHTLLexer::peekChar() const {
    if (m_readPosition >= m_input.length()) {
        return 0;
    }
    return m_input[m_readPosition];
}

void CHTLLexer::skipWhitespace() {
    while (std::isspace(m_char)) {
        readChar();
    }
}

void CHTLLexer::skipSingleLineComment() {
    while (m_char != '\n' && m_char != 0) {
        readChar();
    }
}

void CHTLLexer::skipMultiLineComment() {
    readChar(); // Consume '*'
    while (m_char != 0) {
        if (m_char == '*' && peekChar() == '/') {
            readChar(); // Consume '*'
            readChar(); // Consume '/'
            return;
        }
        readChar();
    }
}

std::string CHTLLexer::readIdentifier() {
    size_t startPosition = m_position;
    while (isIdentifierChar(m_char)) {
        readChar();
    }
    return m_input.substr(startPosition, m_position - startPosition);
}

std::string CHTLLexer::readNumber() {
    size_t startPosition = m_position;
    while (std::isdigit(m_char)) {
        readChar();
    }
    // After the number, check for units like 'px', '%', 'em', etc.
    if (m_char == '%' || (m_char == 'p' && peekChar() == 'x') || (m_char == 'e' && peekChar() == 'm') || (m_char == 'r' && peekChar() == 'e' && m_input[m_readPosition+1] == 'm')) {
         while (std::isalpha(m_char) || m_char == '%') {
            readChar();
        }
    }
    return m_input.substr(startPosition, m_position - startPosition);
}

std::string CHTLLexer::readString(char quote) {
    size_t startPosition = m_position + 1;
    do {
        readChar();
    } while (m_char != quote && m_char != 0);

    std::string result = m_input.substr(startPosition, m_position - startPosition);
    readChar(); // Consume closing quote
    return result;
}

std::string CHTLLexer::readUnquotedLiteral() {
    size_t startPosition = m_position;
    // Unquoted literals end at a delimiter or whitespace before a delimiter.
    // Delimiters: ; { } : =
    while (m_char != 0 && !std::isspace(m_char) && m_char != ';' && m_char != '{' && m_char != '}' && m_char != ':' && m_char != '=') {
        readChar();
    }
    return m_input.substr(startPosition, m_position - startPosition);
}


Token CHTLLexer::NextToken() {
    Token tok;

    skipWhitespace();

    tok.line = m_line;
    tok.column = m_col;

    switch (m_char) {
        case '=':
            tok = {TokenType::TOKEN_ASSIGN, "=", m_line, m_col};
            break;
        case ':':
            tok = {TokenType::TOKEN_COLON, ":", m_line, m_col};
            break;
        case ';':
            tok = {TokenType::TOKEN_SEMICOLON, ";", m_line, m_col};
            break;
        case '(':
            tok = {TokenType::TOKEN_LPAREN, "(", m_line, m_col};
            break;
        case ')':
            tok = {TokenType::TOKEN_RPAREN, ")", m_line, m_col};
            break;
        case '{':
            tok = {TokenType::TOKEN_LBRACE, "{", m_line, m_col};
            break;
        case '}':
            tok = {TokenType::TOKEN_RBRACE, "}", m_line, m_col};
            break;
        case '[':
            tok = {TokenType::TOKEN_LBRACKET, "[", m_line, m_col};
            break;
        case ']':
            tok = {TokenType::TOKEN_RBRACKET, "]", m_line, m_col};
            break;
        case ',':
            tok = {TokenType::TOKEN_COMMA, ",", m_line, m_col};
            break;
        case '.':
            tok = {TokenType::TOKEN_DOT, ".", m_line, m_col};
            break;
        case '#':
            tok = {TokenType::TOKEN_HASH, "#", m_line, m_col};
            break;
        case '&':
            if (peekChar() == '&') {
                readChar();
                tok = {TokenType::TOKEN_AND, "&&", m_line, m_col};
            } else {
                tok = {TokenType::TOKEN_AMPERSAND, "&", m_line, m_col};
            }
            break;
        case '|':
             if (peekChar() == '|') {
                readChar();
                tok = {TokenType::TOKEN_OR, "||", m_line, m_col};
            } else {
                tok = {TokenType::TOKEN_ILLEGAL, "|", m_line, m_col};
            }
            break;
        case '+':
            tok = {TokenType::TOKEN_PLUS, "+", m_line, m_col};
            break;
        case '-':
            tok = {TokenType::TOKEN_MINUS, "-", m_line, m_col};
            break;
        case '/':
            if (peekChar() == '/') {
                skipSingleLineComment();
                return NextToken(); // Recursively get the next token after the comment
            } else if (peekChar() == '*') {
                skipMultiLineComment();
                return NextToken(); // Recursively get the next token
            }
            tok = {TokenType::TOKEN_SLASH, "/", m_line, m_col};
            break;
        case '*':
            if (peekChar() == '*') {
                readChar();
                tok = {TokenType::TOKEN_POWER, "**", m_line, m_col};
            } else {
                tok = {TokenType::TOKEN_STAR, "*", m_line, m_col};
            }
            break;
        case '%':
            tok = {TokenType::TOKEN_PERCENT, "%", m_line, m_col};
            break;
        case '<':
            tok = {TokenType::TOKEN_LT, "<", m_line, m_col};
            break;
        case '>':
            tok = {TokenType::TOKEN_GT, ">", m_line, m_col};
            break;
        case '?':
            tok = {TokenType::TOKEN_QUESTION, "?", m_line, m_col};
            break;
        case '@':
            tok = {TokenType::TOKEN_AT, "@", m_line, m_col};
            break;
        case '"':
        case '\'':
            tok.type = TokenType::TOKEN_STRING;
            tok.literal = readString(m_char);
            return tok; // Return early as readString consumes the closing quote
        case 0:
            tok = {TokenType::TOKEN_EOF, "", m_line, m_col};
            break;
        default:
            if (std::isalpha(m_char) || m_char == '_') {
                tok.literal = readIdentifier();
                tok.type = TokenType::TOKEN_IDENTIFIER;
                return tok;
            } else if (std::isdigit(m_char)) {
                tok.literal = readNumber();
                tok.type = TokenType::TOKEN_IDENTIFIER; // Treat numbers/dims as identifiers for now
                return tok;
            } else {
                tok = {TokenType::TOKEN_ILLEGAL, std::string(1, m_char), m_line, m_col};
            }
            break;
    }

    readChar();
    return tok;
}

} // namespace CHTL
