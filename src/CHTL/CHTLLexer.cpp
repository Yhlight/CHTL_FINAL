#include "CHTL/CHTLLexer.h"
#include <cctype>
#include <algorithm>
#include <unordered_map>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& source) 
    : m_source(source), m_position(0), m_line(1), m_column(1) {
}

CHTLLexer::~CHTLLexer() = default;

Token CHTLLexer::nextToken() {
    skipWhitespace();
    
    if (m_position >= m_source.length()) {
        return Token(TokenType::END_OF_FILE, "", m_line, m_column);
    }
    
    char current = currentChar();
    
    // 处理注释
    if (current == '/' && m_position + 1 < m_source.length()) {
        char next = m_source[m_position + 1];
        if (next == '/') {
            skipLineComment();
            return nextToken();
        } else if (next == '*') {
            skipBlockComment();
            return nextToken();
        }
    }
    
    // 处理生成器注释
    if (current == '-' && m_position + 1 < m_source.length() && m_source[m_position + 1] == '-') {
        skipGeneratorComment();
        return nextToken();
    }
    
    // 处理字符串
    if (current == '"' || current == '\'') {
        return parseString();
    }
    
    // 处理数字
    if (isDigit(current)) {
        return parseNumber();
    }
    
    // 处理标识符和关键字
    if (isLetter(current) || current == '_') {
        return parseIdentifier();
    }
    
    // 处理字面量（无修饰字符串）
    if (current != ' ' && current != '\t' && current != '\n' && current != '\r' && 
        current != '{' && current != '}' && current != '[' && current != ']' &&
        current != '(' && current != ')' && current != ';' && current != ':' &&
        current != '=' && current != ',' && current != '.' && current != '#' &&
        current != '&' && current != '>' && current != '<' && current != '?' &&
        current != '!' && current != '|' && current != '-' && current != '+') {
        return parseLiteral();
    }
    
    // 处理符号
    switch (current) {
        case '{':
            nextChar();
            return Token(TokenType::LEFT_BRACE, "{", m_line, m_column - 1);
        case '}':
            nextChar();
            return Token(TokenType::RIGHT_BRACE, "}", m_line, m_column - 1);
        case '[':
            nextChar();
            return Token(TokenType::LEFT_BRACKET, "[", m_line, m_column - 1);
        case ']':
            nextChar();
            return Token(TokenType::RIGHT_BRACKET, "]", m_line, m_column - 1);
        case '(':
            nextChar();
            return Token(TokenType::LEFT_PAREN, "(", m_line, m_column - 1);
        case ')':
            nextChar();
            return Token(TokenType::RIGHT_PAREN, ")", m_line, m_column - 1);
        case ';':
            nextChar();
            return Token(TokenType::SEMICOLON, ";", m_line, m_column - 1);
        case ':':
            nextChar();
            return Token(TokenType::COLON, ":", m_line, m_column - 1);
        case '=':
            nextChar();
            return Token(TokenType::EQUALS, "=", m_line, m_column - 1);
        case ',':
            nextChar();
            return Token(TokenType::COMMA, ",", m_line, m_column - 1);
        case '.':
            nextChar();
            return Token(TokenType::DOT, ".", m_line, m_column - 1);
        case '#':
            nextChar();
            return Token(TokenType::HASH, "#", m_line, m_column - 1);
        case '&':
            if (m_position + 1 < m_source.length() && m_source[m_position + 1] == '&') {
                nextChar();
                nextChar();
                return Token(TokenType::AND, "&&", m_line, m_column - 2);
            }
            nextChar();
            return Token(TokenType::AMPERSAND, "&", m_line, m_column - 1);
        case '>':
            if (m_position + 1 < m_source.length() && m_source[m_position + 1] == '=') {
                nextChar();
                nextChar();
                return Token(TokenType::GREATER_EQUAL, ">=", m_line, m_column - 2);
            }
            nextChar();
            return Token(TokenType::GREATER, ">", m_line, m_column - 1);
        case '<':
            if (m_position + 1 < m_source.length() && m_source[m_position + 1] == '=') {
                nextChar();
                nextChar();
                return Token(TokenType::LESS_EQUAL, "<=", m_line, m_column - 2);
            }
            nextChar();
            return Token(TokenType::LESS, "<", m_line, m_column - 1);
        case '?':
            nextChar();
            return Token(TokenType::QUESTION, "?", m_line, m_column - 1);
        case '!':
            if (m_position + 1 < m_source.length() && m_source[m_position + 1] == '=') {
                nextChar();
                nextChar();
                return Token(TokenType::NOT_EQUAL, "!=", m_line, m_column - 2);
            }
            nextChar();
            return Token(TokenType::UNKNOWN, "!", m_line, m_column - 1);
        case '|':
            if (m_position + 1 < m_source.length() && m_source[m_position + 1] == '|') {
                nextChar();
                nextChar();
                return Token(TokenType::OR, "||", m_line, m_column - 2);
            }
            nextChar();
            return Token(TokenType::UNKNOWN, "|", m_line, m_column - 1);
        case '-':
            if (m_position + 1 < m_source.length() && m_source[m_position + 1] == '>') {
                nextChar();
                nextChar();
                return Token(TokenType::ARROW, "->", m_line, m_column - 2);
            }
            nextChar();
            return Token(TokenType::UNKNOWN, "-", m_line, m_column - 1);
        default:
            nextChar();
            return Token(TokenType::UNKNOWN, std::string(1, current), m_line, m_column - 1);
    }
}

Token CHTLLexer::peekToken() {
    size_t oldPosition = m_position;
    size_t oldLine = m_line;
    size_t oldColumn = m_column;
    
    Token token = nextToken();
    
    m_position = oldPosition;
    m_line = oldLine;
    m_column = oldColumn;
    
    return token;
}

void CHTLLexer::reset() {
    m_position = 0;
    m_line = 1;
    m_column = 1;
}

size_t CHTLLexer::getCurrentLine() const {
    return m_line;
}

size_t CHTLLexer::getCurrentColumn() const {
    return m_column;
}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    reset();
    
    Token token = nextToken();
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = nextToken();
    }
    tokens.push_back(token); // 添加EOF token
    
    return tokens;
}

char CHTLLexer::currentChar() const {
    return (m_position < m_source.length()) ? m_source[m_position] : '\0';
}

char CHTLLexer::nextChar() {
    if (m_position < m_source.length()) {
        char current = m_source[m_position];
        m_position++;
        if (current == '\n') {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        return current;
    }
    return '\0';
}

void CHTLLexer::skipWhitespace() {
    while (m_position < m_source.length() && isWhitespace(currentChar())) {
        nextChar();
    }
}

void CHTLLexer::skipLineComment() {
    while (m_position < m_source.length() && currentChar() != '\n') {
        nextChar();
    }
}

void CHTLLexer::skipBlockComment() {
    nextChar(); // 跳过 '*'
    while (m_position < m_source.length()) {
        if (currentChar() == '*' && m_position + 1 < m_source.length() && m_source[m_position + 1] == '/') {
            nextChar(); // 跳过 '*'
            nextChar(); // 跳过 '/'
            break;
        }
        nextChar();
    }
}

void CHTLLexer::skipGeneratorComment() {
    nextChar(); // 跳过第二个 '-'
    while (m_position < m_source.length() && currentChar() != '\n') {
        nextChar();
    }
}

Token CHTLLexer::parseIdentifier() {
    std::string value;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    while (m_position < m_source.length() && isAlphaNumeric(currentChar())) {
        value += nextChar();
    }
    
    TokenType type = identifyKeyword(value);
    return Token(type, value, startLine, startColumn);
}

Token CHTLLexer::parseString() {
    std::string value;
    char quote = currentChar();
    nextChar(); // 跳过开始引号
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    while (m_position < m_source.length() && currentChar() != quote) {
        if (currentChar() == '\\' && m_position + 1 < m_source.length()) {
            nextChar(); // 跳过反斜杠
            char escaped = nextChar();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: value += escaped; break;
            }
        } else {
            value += nextChar();
        }
    }
    
    if (m_position < m_source.length()) {
        nextChar(); // 跳过结束引号
    }
    
    return Token(TokenType::STRING, value, startLine, startColumn);
}

Token CHTLLexer::parseNumber() {
    std::string value;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    while (m_position < m_source.length() && (isDigit(currentChar()) || currentChar() == '.')) {
        value += nextChar();
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn);
}

Token CHTLLexer::parseLiteral() {
    std::string value;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    while (m_position < m_source.length() && 
           !isWhitespace(currentChar()) && 
           currentChar() != '{' && currentChar() != '}' &&
           currentChar() != '[' && currentChar() != ']' &&
           currentChar() != '(' && currentChar() != ')' &&
           currentChar() != ';' && currentChar() != ':' &&
           currentChar() != '=' && currentChar() != ',' &&
           currentChar() != '.' && currentChar() != '#' &&
           currentChar() != '&' && currentChar() != '>' &&
           currentChar() != '<' && currentChar() != '?' &&
           currentChar() != '!' && currentChar() != '|' &&
           currentChar() != '-' && currentChar() != '+') {
        value += nextChar();
    }
    
    return Token(TokenType::LITERAL, value, startLine, startColumn);
}

bool CHTLLexer::isLetter(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isLetter(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

TokenType CHTLLexer::identifyKeyword(const std::string& value) const {
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
        {"@Style", TokenType::AT_STYLE},
        {"@Element", TokenType::AT_ELEMENT},
        {"@Var", TokenType::AT_VAR},
        {"@Html", TokenType::AT_HTML},
        {"@JavaScript", TokenType::AT_JAVASCRIPT},
        {"@Chtl", TokenType::AT_CHTL},
        {"@Config", TokenType::AT_CONFIG},
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"at", TokenType::AT_TOP}, // 简化处理
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"except", TokenType::EXCEPT},
        {"use", TokenType::USE},
        {"html5", TokenType::HTML5}
    };
    
    auto it = keywords.find(value);
    return (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
}

TokenType CHTLLexer::identifyOperator(const std::string& value) const {
    static const std::unordered_map<std::string, TokenType> operators = {
        {">", TokenType::GREATER},
        {"<", TokenType::LESS},
        {">=", TokenType::GREATER_EQUAL},
        {"<=", TokenType::LESS_EQUAL},
        {"==", TokenType::EQUAL},
        {"!=", TokenType::NOT_EQUAL},
        {"&&", TokenType::AND},
        {"||", TokenType::OR},
        {"?", TokenType::QUESTION},
        {"->", TokenType::ARROW}
    };
    
    auto it = operators.find(value);
    return (it != operators.end()) ? it->second : TokenType::UNKNOWN;
}

} // namespace CHTL