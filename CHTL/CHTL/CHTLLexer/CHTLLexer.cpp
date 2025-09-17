#include "CHTLLexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLLexer::CHTLLexer() 
    : m_position(0), m_line(1), m_column(1), 
      m_keepComments(false), m_keepWhitespace(false),
      m_inString(false), m_inComment(false), m_stringDelimiter(0) {
    KeywordMap::initialize();
}

CHTLLexer::~CHTLLexer() {
}

std::vector<Token> CHTLLexer::tokenize(const std::string& code) {
    reset();
    m_code = code;
    
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = getNextToken();
        if (token.type != TokenType::UNKNOWN) {
            tokens.push_back(token);
        }
    }
    
    // 添加文件结束标记
    tokens.emplace_back(TokenType::END_OF_FILE, "", m_line, m_column, m_position);
    
    return tokens;
}

void CHTLLexer::reset() {
    m_position = 0;
    m_line = 1;
    m_column = 1;
    m_inString = false;
    m_inComment = false;
    m_stringDelimiter = 0;
}

void CHTLLexer::setKeepComments(bool keep) {
    m_keepComments = keep;
}

void CHTLLexer::setKeepWhitespace(bool keep) {
    m_keepWhitespace = keep;
}

Token CHTLLexer::getNextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE, "", m_line, m_column, m_position);
    }
    
    char c = current();
    
    // 处理注释
    if (c == '/' && peek() == '/') {
        if (m_keepComments) {
            return processGeneratorComment();
        } else {
            skipLineComment();
            return getNextToken();
        }
    }
    
    if (c == '/' && peek() == '*') {
        if (m_keepComments) {
            return processGeneratorComment();
        } else {
            skipBlockComment();
            return getNextToken();
        }
    }
    
    // 处理生成器注释
    if (c == '#') {
        return processGeneratorComment();
    }
    
    // 处理字符串
    if (c == '"' || c == '\'') {
        return processString();
    }
    
    // 处理数字
    if (std::isdigit(c)) {
        return processNumber();
    }
    
    // 处理标识符和关键字
    if (std::isalpha(c) || c == '_') {
        return processIdentifier();
    }
    
    // 处理字面量（无修饰字符串）
    if (std::isalnum(c) || c == '-' || c == '_') {
        return processLiteral();
    }
    
    // 处理操作符和分隔符
    if (TokenUtils::isOperator(c)) {
        return processOperator();
    }
    
    if (TokenUtils::isDelimiter(c)) {
        return processDelimiter();
    }
    
    // 未知字符
    advance();
    return Token(TokenType::UNKNOWN, std::string(1, c), m_line, m_column - 1, m_position - 1);
}

void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && std::isspace(current())) {
        if (current() == '\n') {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        advance();
    }
}

void CHTLLexer::skipComment() {
    if (current() == '/' && peek() == '/') {
        skipLineComment();
    } else if (current() == '/' && peek() == '*') {
        skipBlockComment();
    }
}

void CHTLLexer::skipLineComment() {
    while (!isAtEnd() && current() != '\n') {
        advance();
    }
    if (!isAtEnd()) {
        advance(); // 跳过换行符
        m_line++;
        m_column = 1;
    }
}

void CHTLLexer::skipBlockComment() {
    advance(); // 跳过 /
    advance(); // 跳过 *
    
    while (!isAtEnd()) {
        if (current() == '*' && peek() == '/') {
            advance(); // 跳过 *
            advance(); // 跳过 /
            break;
        }
        if (current() == '\n') {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        advance();
    }
}

Token CHTLLexer::processGeneratorComment() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    std::string value;
    
    if (current() == '#') {
        advance(); // 跳过 #
        value += '#';
        
        // 跳过空格
        while (!isAtEnd() && std::isspace(current())) {
            advance();
        }
        
        // 读取注释内容
        while (!isAtEnd() && current() != '\n') {
            value += current();
            advance();
        }
    } else if (current() == '/' && peek() == '/') {
        advance(); // 跳过第一个 /
        advance(); // 跳过第二个 /
        value = "//";
        
        while (!isAtEnd() && current() != '\n') {
            value += current();
            advance();
        }
    } else if (current() == '/' && peek() == '*') {
        advance(); // 跳过 /
        advance(); // 跳过 *
        value = "/*";
        
        while (!isAtEnd()) {
            if (current() == '*' && peek() == '/') {
                advance(); // 跳过 *
                advance(); // 跳过 /
                value += "*/";
                break;
            }
            value += current();
            advance();
        }
    }
    
    return Token(TokenType::GENERATOR_COMMENT, value, startLine, startColumn, startPos);
}

Token CHTLLexer::processIdentifier() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    std::string value;
    
    while (!isAtEnd() && (std::isalnum(current()) || current() == '_')) {
        value += current();
        advance();
    }
    
    // 检查是否为关键字
    TokenType type = KeywordMap::getKeywordType(value);
    if (type != TokenType::UNKNOWN) {
        return Token(type, value, startLine, startColumn, startPos);
    }
    
    return Token(TokenType::IDENTIFIER, value, startLine, startColumn, startPos);
}

Token CHTLLexer::processString() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    char delimiter = current();
    advance(); // 跳过开始引号
    
    std::string value;
    value += delimiter;
    
    while (!isAtEnd() && current() != delimiter) {
        if (current() == '\\' && peek() != '\0') {
            advance(); // 跳过反斜杠
            value += '\\';
            value += current();
        } else {
            value += current();
        }
        advance();
    }
    
    if (!isAtEnd()) {
        advance(); // 跳过结束引号
        value += delimiter;
    }
    
    return Token(TokenType::STRING, value, startLine, startColumn, startPos);
}

Token CHTLLexer::processLiteral() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    std::string value;
    
    while (!isAtEnd() && (std::isalnum(current()) || current() == '-' || current() == '_' || current() == '.')) {
        value += current();
        advance();
    }
    
    return Token(TokenType::LITERAL, value, startLine, startColumn, startPos);
}

Token CHTLLexer::processNumber() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    std::string value;
    
    while (!isAtEnd() && (std::isdigit(current()) || current() == '.')) {
        value += current();
        advance();
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn, startPos);
}

Token CHTLLexer::processOperator() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    char c = current();
    advance();
    
    // 处理双字符操作符
    if (c == '=' && current() == '=') {
        advance();
        return Token(TokenType::EQUALS, "==", startLine, startColumn, startPos);
    }
    
    if (c == '!' && current() == '=') {
        advance();
        return Token(TokenType::UNKNOWN, "!=", startLine, startColumn, startPos);
    }
    
    if (c == '<' && current() == '=') {
        advance();
        return Token(TokenType::UNKNOWN, "<=", startLine, startColumn, startPos);
    }
    
    if (c == '>' && current() == '=') {
        advance();
        return Token(TokenType::UNKNOWN, ">=", startLine, startColumn, startPos);
    }
    
    if (c == '&' && current() == '&') {
        advance();
        return Token(TokenType::UNKNOWN, "&&", startLine, startColumn, startPos);
    }
    
    if (c == '|' && current() == '|') {
        advance();
        return Token(TokenType::UNKNOWN, "||", startLine, startColumn, startPos);
    }
    
    // 单字符操作符
    switch (c) {
        case '+':
            return Token(TokenType::UNKNOWN, "+", startLine, startColumn, startPos);
        case '-':
            return Token(TokenType::UNKNOWN, "-", startLine, startColumn, startPos);
        case '*':
            return Token(TokenType::UNKNOWN, "*", startLine, startColumn, startPos);
        case '/':
            return Token(TokenType::UNKNOWN, "/", startLine, startColumn, startPos);
        case '%':
            return Token(TokenType::UNKNOWN, "%", startLine, startColumn, startPos);
        case '^':
            return Token(TokenType::UNKNOWN, "^", startLine, startColumn, startPos);
        case '?':
            return Token(TokenType::UNKNOWN, "?", startLine, startColumn, startPos);
        case '!':
            return Token(TokenType::UNKNOWN, "!", startLine, startColumn, startPos);
        case '<':
            return Token(TokenType::UNKNOWN, "<", startLine, startColumn, startPos);
        case '>':
            return Token(TokenType::UNKNOWN, ">", startLine, startColumn, startPos);
        default:
            return Token(TokenType::UNKNOWN, std::string(1, c), startLine, startColumn, startPos);
    }
}

Token CHTLLexer::processDelimiter() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    char c = current();
    advance();
    
    switch (c) {
        case '{':
            return Token(TokenType::LEFT_BRACE, "{", startLine, startColumn, startPos);
        case '}':
            return Token(TokenType::RIGHT_BRACE, "}", startLine, startColumn, startPos);
        case '[':
            return Token(TokenType::LEFT_BRACKET, "[", startLine, startColumn, startPos);
        case ']':
            return Token(TokenType::RIGHT_BRACKET, "]", startLine, startColumn, startPos);
        case '(':
            return Token(TokenType::LEFT_PAREN, "(", startLine, startColumn, startPos);
        case ')':
            return Token(TokenType::RIGHT_PAREN, ")", startLine, startColumn, startPos);
        case ';':
            return Token(TokenType::SEMICOLON, ";", startLine, startColumn, startPos);
        case ':':
            return Token(TokenType::COLON, ":", startLine, startColumn, startPos);
        case '=':
            return Token(TokenType::EQUALS, "=", startLine, startColumn, startPos);
        case ',':
            return Token(TokenType::COMMA, ",", startLine, startColumn, startPos);
        case '.':
            return Token(TokenType::DOT, ".", startLine, startColumn, startPos);
        case '#':
            return Token(TokenType::HASH, "#", startLine, startColumn, startPos);
        default:
            return Token(TokenType::UNKNOWN, std::string(1, c), startLine, startColumn, startPos);
    }
}

bool CHTLLexer::isKeyword(const std::string& str) {
    return KeywordMap::isKeyword(str);
}

char CHTLLexer::current() const {
    if (isAtEnd()) return '\0';
    return m_code[m_position];
}

char CHTLLexer::peek(size_t offset) const {
    if (m_position + offset >= m_code.length()) return '\0';
    return m_code[m_position + offset];
}

void CHTLLexer::advance() {
    if (!isAtEnd()) {
        m_position++;
        m_column++;
    }
}

bool CHTLLexer::isAtEnd() const {
    return m_position >= m_code.length();
}

std::pair<size_t, size_t> CHTLLexer::getPosition() const {
    return std::make_pair(m_line, m_column);
}

} // namespace CHTL