#include "Lexer.h"
#include <cctype>
#include <unordered_map>

namespace CHTL {

// Helper to check if a character can be part of an identifier
bool isIdentifierChar(char ch) {
    return std::isalnum(ch) || ch == '_';
}

// Keyword map
static const std::unordered_map<std::string, TokenType> keywords = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"inherit", TokenType::INHERIT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"at", TokenType::AT},
    {"top", TokenType::TOP},
    {"bottom", TokenType::BOTTOM},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"except", TokenType::EXCEPT},
    {"use", TokenType::USE},
    {"html5", TokenType::HTML5},
    {"[Custom]", TokenType::KEYWORD_CUSTOM},
    {"[Template]", TokenType::KEYWORD_TEMPLATE},
    {"[Origin]", TokenType::KEYWORD_ORIGIN},
    {"[Import]", TokenType::KEYWORD_IMPORT},
    {"[Namespace]", TokenType::KEYWORD_NAMESPACE},
    {"[Configuration]", TokenType::KEYWORD_CONFIGURATION},
    {"[Info]", TokenType::KEYWORD_INFO},
    {"[Export]", TokenType::KEYWORD_EXPORT},
    {"[Name]", TokenType::KEYWORD_NAME},
    {"[OriginType]", TokenType::KEYWORD_ORIGINTYPE},
};

Lexer::Lexer(std::string input)
    : m_input(std::move(input)), m_position(0), m_readPosition(0), m_ch(0), m_currentLine(1) {
    readChar();
}

void Lexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_ch = 0; // EOF
    } else {
        m_ch = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition += 1;
}

char Lexer::peekChar() const {
    if (m_readPosition >= m_input.length()) {
        return 0;
    }
    return m_input[m_readPosition];
}

void Lexer::skipWhitespace() {
    while (std::isspace(m_ch)) {
        if (m_ch == '\n') {
            m_currentLine++;
        }
        readChar();
    }
}

Token Lexer::nextToken() {
    Token tok;
    skipWhitespace();
    tok.line = m_currentLine;

    switch (m_ch) {
        case '=': tok = {TokenType::EQUALS, "=", tok.line}; break;
        case ';': tok = {TokenType::SEMICOLON, ";", tok.line}; break;
        case ':': tok = {TokenType::COLON, ":", tok.line}; break;
        case ',': tok = {TokenType::COMMA, ",", tok.line}; break;
        case '.': tok = {TokenType::DOT, ".", tok.line}; break;
        case '?': tok = {TokenType::QUESTION, "?", tok.line}; break;
        case '(': tok = {TokenType::LPAREN, "(", tok.line}; break;
        case ')': tok = {TokenType::RPAREN, ")", tok.line}; break;
        case '{': tok = {TokenType::LBRACE, "{", tok.line}; break;
        case '}': tok = {TokenType::RBRACE, "}", tok.line}; break;
        case '+': tok = {TokenType::PLUS, "+", tok.line}; break;
        case '%': tok = {TokenType::PERCENT, "%", tok.line}; break;
        case '&':
            if (peekChar() == '&') {
                readChar();
                tok = {TokenType::LOGICAL_AND, "&&", tok.line};
            } else {
                tok = {TokenType::AMPERSAND, "&", tok.line};
            }
            break;
        case '|':
            if (peekChar() == '|') {
                readChar();
                tok = {TokenType::LOGICAL_OR, "||", tok.line};
            } else {
                tok = {TokenType::ILLEGAL, "|", tok.line};
            }
            break;
        case '*':
            if (peekChar() == '*') {
                readChar();
                tok = {TokenType::POWER, "**", tok.line};
            } else {
                tok = {TokenType::ASTERISK, "*", tok.line};
            }
            break;
        case '-':
            if (peekChar() == '-') {
                readChar();
                tok = {TokenType::GEN_COMMENT, "--", tok.line};
            } else {
                tok = {TokenType::MINUS, "-", tok.line};
            }
            break;
        case '/':
            if (peekChar() == '/') {
                readChar();
                tok = {TokenType::LINE_COMMENT, "//", tok.line};
            } else if (peekChar() == '*') {
                 readChar();
                 tok = {TokenType::BLOCK_COMMENT, "/*", tok.line};
            }
            else {
                tok = {TokenType::SLASH, "/", tok.line};
            }
            break;
        case '"':
        case '\'':
            tok.literal = readStringLiteral(m_ch);
            tok.type = TokenType::STRING_LITERAL;
            break;
        case '@': {
            readChar(); // consume '@'
            std::string ident = readIdentifier();
            if (ident == "Style") tok = {TokenType::TYPE_STYLE, "@Style", tok.line};
            else if (ident == "Element") tok = {TokenType::TYPE_ELEMENT, "@Element", tok.line};
            else if (ident == "Var") tok = {TokenType::TYPE_VAR, "@Var", tok.line};
            else if (ident == "Html") tok = {TokenType::TYPE_HTML, "@Html", tok.line};
            else if (ident == "Chtl") tok = {TokenType::TYPE_CHTL, "@Chtl", tok.line};
            else tok = {TokenType::ILLEGAL, "@" + ident, tok.line};
            return tok; // Early return
        }
        case '[':
            tok = {TokenType::LBRACKET, "[", tok.line};
            break;
        case ']':
            tok = {TokenType::RBRACKET, "]", tok.line};
            break;
        case 0:
            tok = {TokenType::EOF_TOKEN, "", tok.line};
            break;
        default:
            if (isalpha(m_ch) || m_ch == '_') {
                tok.literal = readIdentifier();
                tok.type = lookupIdent(tok.literal);
                return tok;
            } else if (isdigit(m_ch)) {
                tok.literal = readNumberLiteral();
                tok.type = TokenType::NUMBER_LITERAL;
                return tok;
            } else {
                tok = {TokenType::ILLEGAL, std::string(1, m_ch), tok.line};
            }
    }

    readChar();
    return tok;
}


std::string Lexer::readIdentifier() {
    size_t start_pos = m_position;
    while (isIdentifierChar(m_ch)) {
        readChar();
    }
    return m_input.substr(start_pos, m_position - start_pos);
}

std::string Lexer::readStringLiteral(char quote_type) {
    size_t start_pos = m_position + 1;
    readChar();
    while (m_ch != quote_type && m_ch != 0) {
        if (m_ch == '\\') {
            readChar();
        }
        readChar();
    }
    return m_input.substr(start_pos, m_position - start_pos);
}

std::string Lexer::readNumberLiteral() {
    size_t start_pos = m_position;
    while (isdigit(m_ch) || m_ch == '.') {
        readChar();
    }
    return m_input.substr(start_pos, m_position - start_pos);
}

TokenType Lexer::lookupIdent(const std::string& ident) {
    if (keywords.count(ident)) {
        return keywords.at(ident);
    }
    return TokenType::IDENTIFIER;
}

} // namespace CHTL
