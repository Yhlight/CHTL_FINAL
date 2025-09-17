#include "Lexer.h"
#include <cctype>

namespace CHTL {

const std::unordered_map<std::string, TokenType> Lexer::s_keywords = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"Template", TokenType::KEYWORD_TEMPLATE},
    {"Custom", TokenType::KEYWORD_CUSTOM},
    {"Style", TokenType::KEYWORD_STYLE},
    {"Element", TokenType::KEYWORD_ELEMENT},
    {"Var", TokenType::KEYWORD_VAR},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"at", TokenType::KEYWORD_AT},
    {"top", TokenType::KEYWORD_TOP},
    {"bottom", TokenType::KEYWORD_BOTTOM}
};

Lexer::Lexer(const std::string& source) : m_source(source) {}

std::vector<Token> Lexer::getAllTokens() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = getNextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);
    return tokens;
}

Token Lexer::getNextToken() {
    skipWhitespace();

    if (m_current >= m_source.length()) {
        return makeToken(TokenType::END_OF_FILE, "");
    }

    char c = m_source[m_current];

    if (std::isalpha(c) || c == '_') {
        return identifier();
    }

    if (c == '"' || c == '\'') {
        return stringLiteral(c);
    }

    if (c == '#') {
        m_current++; // consume '#'
        std::string comment_value;
        while(peek() != '\n' && m_current < m_source.length()) {
            comment_value += peek();
            advance();
        }
        return makeToken(TokenType::COMMENT, "#" + comment_value);
    }

    if (c == '/') {
        if (peekNext() == '/') {
            skipLineComment();
            return getNextToken();
        } else if (peekNext() == '*') {
            skipBlockComment();
            return getNextToken();
        }
    }


    switch (c) {
        case '{': advance(); return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': advance(); return makeToken(TokenType::RIGHT_BRACE, "}");
        case '[': advance(); return makeToken(TokenType::LEFT_BRACKET, "[");
        case ']': advance(); return makeToken(TokenType::RIGHT_BRACKET, "]");
        case '(': advance(); return makeToken(TokenType::LEFT_PAREN, "(");
        case ')': advance(); return makeToken(TokenType::RIGHT_PAREN, ")");
        case '@': advance(); return makeToken(TokenType::AT_SIGN, "@");
        case ':': advance(); return makeToken(TokenType::COLON, ":");
        case ';': advance(); return makeToken(TokenType::SEMICOLON, ";");
        case '=': advance(); return makeToken(TokenType::EQUALS, "=");
        case ',': advance(); return makeToken(TokenType::COMMA, ",");
    }

    if (!std::isspace(c) && c != '{' && c != '}' && c != ':' && c != ';') {
        return unquotedLiteral();
    }


    advance();
    return makeToken(TokenType::UNKNOWN, std::string(1, c));
}

void Lexer::advance() {
    if (m_current < m_source.length()) {
        if (m_source[m_current] == '\n') {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        m_current++;
    }
}

char Lexer::peek() {
    if (m_current >= m_source.length()) return '\0';
    return m_source[m_current];
}

char Lexer::peekNext() {
    if (m_current + 1 >= m_source.length()) return '\0';
    return m_source[m_current + 1];
}

void Lexer::skipWhitespace() {
    while (m_current < m_source.length() && std::isspace(peek())) {
        advance();
    }
}

void Lexer::skipLineComment() {
    while (peek() != '\n' && m_current < m_source.length()) {
        advance();
    }
}

void Lexer::skipBlockComment() {
    advance();
    advance();
    while (m_current < m_source.length() && (peek() != '*' || peekNext() != '/')) {
        advance();
    }
    if (m_current < m_source.length()) {
        advance();
        advance();
    }
}

Token Lexer::identifier() {
    std::string value;
    size_t start_col = m_column;
    while (std::isalnum(peek()) || peek() == '_' || peek() == '-') {
        value += peek();
        advance();
    }

    if (s_keywords.count(value)) {
        return Token{s_keywords.at(value), value, m_line, (int)start_col};
    }
    return Token{TokenType::IDENTIFIER, value, m_line, (int)start_col};
}

Token Lexer::stringLiteral(char quoteType) {
    std::string value;
    size_t start_col = m_column;
    advance(); // consume opening quote
    while (peek() != quoteType && m_current < m_source.length()) {
        if (peek() == '\\' && peekNext() == quoteType) {
            advance();
        }
        value += peek();
        advance();
    }
    advance(); // consume closing quote
    return Token{TokenType::STRING_LITERAL, value, m_line, (int)start_col};
}

Token Lexer::unquotedLiteral() {
    std::string value;
    size_t start_col = m_column;
    while (m_current < m_source.length()) {
        char c = peek();
        if (std::isspace(c) || c == '{' || c == '}' || c == ';' || c == ':' || c == '[' || c == ']' || c == '(' || c == ')') {
            break;
        }
        value += c;
        advance();
    }
    return Token{TokenType::UNQUOTED_LITERAL, value, m_line, (int)start_col};
}


Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token{type, value, m_line, m_column};
}

} // namespace CHTL
