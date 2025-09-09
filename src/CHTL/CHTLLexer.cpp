#include "CHTL/CHTLLexer.h"
#include <cctype>
#include <unordered_map>

namespace CHTL {

CHTLLexer::CHTLLexer(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_position(0), m_line(1), m_column(1) {
}

std::vector<Token> CHTLLexer::tokenize(const std::string& source) {
    m_source = source;
    m_position = 0;
    m_line = 1;
    m_column = 1;
    
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = scanToken();
        if (token.type != TokenType::UNKNOWN) {
            tokens.push_back(token);
        }
    }
    
    tokens.emplace_back(TokenType::EOF_TOKEN, "", m_line, m_column);
    return tokens;
}

void CHTLLexer::reset() {
    m_position = 0;
    m_line = 1;
    m_column = 1;
}

char CHTLLexer::current() const {
    if (isAtEnd()) return '\0';
    return m_source[m_position];
}

char CHTLLexer::peek(size_t offset) const {
    if (m_position + offset >= m_source.length()) return '\0';
    return m_source[m_position + offset];
}

void CHTLLexer::advance(size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (current() == '\n') {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        m_position++;
    }
}

bool CHTLLexer::isAtEnd() const {
    return m_position >= m_source.length();
}

Token CHTLLexer::scanToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", m_line, m_column);
    }
    
    char c = current();
    
    // 单字符符号
    switch (c) {
        case '{': advance(); return Token(TokenType::LEFT_BRACE, "{", m_line, m_column - 1);
        case '}': advance(); return Token(TokenType::RIGHT_BRACE, "}", m_line, m_column - 1);
        case '[': advance(); return Token(TokenType::LEFT_BRACKET, "[", m_line, m_column - 1);
        case ']': advance(); return Token(TokenType::RIGHT_BRACKET, "]", m_line, m_column - 1);
        case '(': advance(); return Token(TokenType::LEFT_PAREN, "(", m_line, m_column - 1);
        case ')': advance(); return Token(TokenType::RIGHT_PAREN, ")", m_line, m_column - 1);
        case ';': advance(); return Token(TokenType::SEMICOLON, ";", m_line, m_column - 1);
        case ',': advance(); return Token(TokenType::COMMA, ",", m_line, m_column - 1);
        case '.': advance(); return Token(TokenType::DOT, ".", m_line, m_column - 1);
        case '#': advance(); return Token(TokenType::HASH, "#", m_line, m_column - 1);
        case '@': advance(); return Token(TokenType::AT, "@", m_line, m_column - 1);
        case '&': advance(); return Token(TokenType::AMPERSAND, "&", m_line, m_column - 1);
        case '?': advance(); return Token(TokenType::QUESTION, "?", m_line, m_column - 1);
        case '!': advance(); return Token(TokenType::EXCLAMATION, "!", m_line, m_column - 1);
        case '_': advance(); return Token(TokenType::UNDERSCORE, "_", m_line, m_column - 1);
    }
    
    // 双字符符号
    if (c == ':' && peek() == ':') {
        advance(2);
        return Token(TokenType::COLON, "::", m_line, m_column - 2);
    } else if (c == ':') {
        advance();
        return Token(TokenType::COLON, ":", m_line, m_column - 1);
    } else if (c == '=' && peek() == '=') {
        advance(2);
        return Token(TokenType::EQUAL, "==", m_line, m_column - 2);
    } else if (c == '=') {
        advance();
        return Token(TokenType::EQUAL, "=", m_line, m_column - 1);
    }
    
    // 注释
    if (c == '/' && peek() == '/') {
        return scanLineComment();
    } else if (c == '/' && peek() == '*') {
        return scanBlockComment();
    } else if (c == '-' && peek() == '-') {
        return scanGeneratorComment();
    }
    
    // 字符串
    if (c == '"' || c == '\'') {
        return scanString();
    }
    
    // 数字
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // 标识符或关键字
    if (isAlpha(c) || c == '_') {
        return scanIdentifier();
    }
    
    // 无修饰字面量（以字母开头且不包含特殊字符）
    if (isAlpha(c)) {
        return scanLiteral();
    }
    
    // 未知字符
    advance();
    return Token(TokenType::UNKNOWN, std::string(1, c), m_line, m_column - 1);
}

Token CHTLLexer::scanIdentifier() {
    int start = m_position;
    while (isAlphaNumeric(current()) || current() == '_') {
        advance();
    }
    
    std::string text = m_source.substr(start, m_position - start);
    TokenType type = getKeywordType(text);
    
    return Token(type, text, m_line, m_column - text.length());
}

Token CHTLLexer::scanString() {
    char quote = current();
    advance(); // 跳过开始引号
    
    int start = m_position;
    while (current() != quote && !isAtEnd()) {
        if (current() == '\\') {
            advance(); // 跳过转义字符
        }
        advance();
    }
    
    if (isAtEnd()) {
        m_context->addError("Unterminated string", m_line, m_column);
        return Token(TokenType::UNKNOWN, "", m_line, m_column);
    }
    
    std::string value = m_source.substr(start, m_position - start);
    advance(); // 跳过结束引号
    
    return Token(TokenType::STRING, value, m_line, m_column - value.length() - 2);
}

Token CHTLLexer::scanNumber() {
    int start = m_position;
    
    while (isDigit(current())) {
        advance();
    }
    
    // 处理小数点
    if (current() == '.' && isDigit(peek())) {
        advance();
        while (isDigit(current())) {
            advance();
        }
    }
    
    std::string value = m_source.substr(start, m_position - start);
    return Token(TokenType::NUMBER, value, m_line, m_column - value.length());
}

Token CHTLLexer::scanLiteral() {
    int start = m_position;
    
    while (isAlphaNumeric(current()) || current() == '_' || current() == '-') {
        advance();
    }
    
    std::string value = m_source.substr(start, m_position - start);
    return Token(TokenType::LITERAL, value, m_line, m_column - value.length());
}

Token CHTLLexer::scanComment() {
    if (current() == '/' && peek() == '/') {
        return scanLineComment();
    } else if (current() == '/' && peek() == '*') {
        return scanBlockComment();
    } else if (current() == '-' && peek() == '-') {
        return scanGeneratorComment();
    }
    
    return Token(TokenType::UNKNOWN, "", m_line, m_column);
}

Token CHTLLexer::scanLineComment() {
    advance(2); // 跳过 //
    
    int start = m_position;
    while (current() != '\n' && !isAtEnd()) {
        advance();
    }
    
    std::string value = m_source.substr(start, m_position - start);
    return Token(TokenType::LINE_COMMENT, value, m_line, m_column - value.length() - 2);
}

Token CHTLLexer::scanBlockComment() {
    advance(2); // 跳过 /*
    
    int start = m_position;
    while (!(current() == '*' && peek() == '/') && !isAtEnd()) {
        advance();
    }
    
    if (isAtEnd()) {
        m_context->addError("Unterminated block comment", m_line, m_column);
        return Token(TokenType::UNKNOWN, "", m_line, m_column);
    }
    
    std::string value = m_source.substr(start, m_position - start);
    advance(2); // 跳过 */
    
    return Token(TokenType::BLOCK_COMMENT, value, m_line, m_column - value.length() - 4);
}

Token CHTLLexer::scanGeneratorComment() {
    advance(2); // 跳过 --
    
    int start = m_position;
    while (current() != '\n' && !isAtEnd()) {
        advance();
    }
    
    std::string value = m_source.substr(start, m_position - start);
    return Token(TokenType::GENERATOR_COMMENT, value, m_line, m_column - value.length() - 2);
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) != 0;
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c) != 0;
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

TokenType CHTLLexer::getKeywordType(const std::string& identifier) const {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        {"Template", TokenType::TEMPLATE},
        {"Custom", TokenType::CUSTOM},
        {"Origin", TokenType::ORIGIN},
        {"Import", TokenType::IMPORT},
        {"Namespace", TokenType::NAMESPACE},
        {"Configuration", TokenType::CONFIGURATION},
        {"Info", TokenType::INFO},
        {"Export", TokenType::EXPORT},
        {"except", TokenType::EXCEPT},
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"use", TokenType::USE},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"at", TokenType::AT_TOP}, // 简化处理
    };
    
    auto it = keywords.find(identifier);
    return (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
}

bool CHTLLexer::isKeyword(const std::string& identifier) const {
    return getKeywordType(identifier) != TokenType::IDENTIFIER;
}

void CHTLLexer::skipWhitespace() {
    while (isWhitespace(current())) {
        if (current() == '\n') {
            skipNewline();
        } else {
            advance();
        }
    }
}

void CHTLLexer::skipNewline() {
    advance();
}

} // namespace CHTL