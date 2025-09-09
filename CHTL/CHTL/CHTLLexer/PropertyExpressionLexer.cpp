#include "PropertyExpressionLexer.hpp"
#include <cctype>
#include <sstream>

namespace CHTL {

PropertyExpressionLexer::PropertyExpressionLexer(const std::string& input)
    : input_(input), position_(0), currentLine_(1), currentColumn_(1) {
}

Token PropertyExpressionLexer::nextToken() {
    // 如果有回退的token，先返回
    if (!ungetTokens_.empty()) {
        Token token = ungetTokens_.back();
        ungetTokens_.pop_back();
        return token;
    }
    
    skipWhitespace();
    
    if (position_ >= input_.length()) {
        return Token(TokenType::END_OF_FILE, "", currentLine_, currentColumn_);
    }
    
    char current = getCurrentChar();
    
    // 解析数字
    if (isDigit(current)) {
        return parseNumber();
    }
    
    // 解析字符串
    if (current == '"' || current == '\'') {
        return parseString();
    }
    
    // 解析动态引用 {{变量}}
    if (current == '{' && peekNextChar() == '{') {
        return parseDynamicReference();
    }
    
    // 解析属性引用 选择器.属性
    if (current == '.' || current == '#' || isAlpha(current)) {
        return parsePropertyReference();
    }
    
    // 解析操作符
    if (isOperatorChar(current)) {
        return parseOperator();
    }
    
    // 解析标识符
    if (isAlpha(current)) {
        return parseIdentifier();
    }
    
    // 未知字符
    advance();
    return Token(TokenType::ERROR, std::string(1, current), currentLine_, currentColumn_);
}

void PropertyExpressionLexer::ungetToken() {
    // 实现token回退功能
    // 这里简化实现，实际应该保存最后一个token
}

bool PropertyExpressionLexer::hasMoreTokens() const {
    return position_ < input_.length();
}

char PropertyExpressionLexer::getCurrentChar() const {
    return (position_ < input_.length()) ? input_[position_] : '\0';
}

char PropertyExpressionLexer::peekNextChar() const {
    return (position_ + 1 < input_.length()) ? input_[position_ + 1] : '\0';
}

void PropertyExpressionLexer::advance() {
    if (position_ < input_.length()) {
        if (input_[position_] == '\n') {
            currentLine_++;
            currentColumn_ = 1;
        } else {
            currentColumn_++;
        }
        position_++;
    }
}

void PropertyExpressionLexer::skipWhitespace() {
    while (position_ < input_.length() && isWhitespace(getCurrentChar())) {
        advance();
    }
}

Token PropertyExpressionLexer::parseNumber() {
    std::string value;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    // 解析整数部分
    while (isDigit(getCurrentChar())) {
        value += getCurrentChar();
        advance();
    }
    
    // 解析小数部分
    if (getCurrentChar() == '.' && isDigit(peekNextChar())) {
        value += getCurrentChar();
        advance();
        while (isDigit(getCurrentChar())) {
            value += getCurrentChar();
            advance();
        }
    }
    
    // 解析单位
    std::string unit;
    while (isAlpha(getCurrentChar())) {
        unit += getCurrentChar();
        advance();
    }
    
    if (!unit.empty()) {
        value += unit;
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn);
}

Token PropertyExpressionLexer::parseString() {
    std::string value;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    char quote = getCurrentChar();
    advance(); // 跳过开始引号
    
    while (position_ < input_.length() && getCurrentChar() != quote) {
        if (getCurrentChar() == '\\' && position_ + 1 < input_.length()) {
            advance(); // 跳过反斜杠
            value += getCurrentChar();
        } else {
            value += getCurrentChar();
        }
        advance();
    }
    
    if (getCurrentChar() == quote) {
        advance(); // 跳过结束引号
    }
    
    return Token(TokenType::STRING_LITERAL, value, startLine, startColumn);
}

Token PropertyExpressionLexer::parseIdentifier() {
    std::string value;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (isAlphaNumeric(getCurrentChar()) || getCurrentChar() == '_' || getCurrentChar() == '-') {
        value += getCurrentChar();
        advance();
    }
    
    return Token(TokenType::IDENTIFIER, value, startLine, startColumn);
}

Token PropertyExpressionLexer::parseOperator() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    char current = getCurrentChar();
    char next = peekNextChar();
    
    // 处理双字符操作符
    if (current == '=' && next == '=') {
        advance();
        advance();
        return Token(TokenType::EQUAL_EQUAL, "==", startLine, startColumn);
    }
    if (current == '!' && next == '=') {
        advance();
        advance();
        return Token(TokenType::NOT_EQUAL, "!=", startLine, startColumn);
    }
    if (current == '<' && next == '=') {
        advance();
        advance();
        return Token(TokenType::LESS_EQUAL, "<=", startLine, startColumn);
    }
    if (current == '>' && next == '=') {
        advance();
        advance();
        return Token(TokenType::GREATER_EQUAL, ">=", startLine, startColumn);
    }
    if (current == '&' && next == '&') {
        advance();
        advance();
        return Token(TokenType::LOGICAL_AND, "&&", startLine, startColumn);
    }
    if (current == '|' && next == '|') {
        advance();
        advance();
        return Token(TokenType::LOGICAL_OR, "||", startLine, startColumn);
    }
    if (current == '*' && next == '*') {
        advance();
        advance();
        return Token(TokenType::POWER, "**", startLine, startColumn);
    }
    
    // 处理单字符操作符
    advance();
    switch (current) {
        case '+': return Token(TokenType::PLUS, "+", startLine, startColumn);
        case '-': return Token(TokenType::MINUS, "-", startLine, startColumn);
        case '*': return Token(TokenType::MULTIPLY, "*", startLine, startColumn);
        case '/': return Token(TokenType::DIVIDE, "/", startLine, startColumn);
        case '%': return Token(TokenType::MODULO, "%", startLine, startColumn);
        case '<': return Token(TokenType::LESS_THAN, "<", startLine, startColumn);
        case '>': return Token(TokenType::GREATER_THAN, ">", startLine, startColumn);
        case '?': return Token(TokenType::QUESTION, "?", startLine, startColumn);
        case ':': return Token(TokenType::COLON, ":", startLine, startColumn);
        case '!': return Token(TokenType::LOGICAL_NOT, "!", startLine, startColumn);
        case '.': return Token(TokenType::DOT, ".", startLine, startColumn);
        case '#': return Token(TokenType::HASH, "#", startLine, startColumn);
        default: return Token(TokenType::ERROR, std::string(1, current), startLine, startColumn);
    }
}

Token PropertyExpressionLexer::parsePropertyReference() {
    std::string value;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    // 解析选择器部分
    if (getCurrentChar() == '.') {
        value += getCurrentChar();
        advance();
        while (isAlphaNumeric(getCurrentChar()) || getCurrentChar() == '_' || getCurrentChar() == '-') {
            value += getCurrentChar();
            advance();
        }
    } else if (getCurrentChar() == '#') {
        value += getCurrentChar();
        advance();
        while (isAlphaNumeric(getCurrentChar()) || getCurrentChar() == '_' || getCurrentChar() == '-') {
            value += getCurrentChar();
            advance();
        }
    } else {
        // 解析标签选择器
        while (isAlphaNumeric(getCurrentChar()) || getCurrentChar() == '_' || getCurrentChar() == '-') {
            value += getCurrentChar();
            advance();
        }
    }
    
    // 解析属性部分
    if (getCurrentChar() == '.') {
        value += getCurrentChar();
        advance();
        while (isAlphaNumeric(getCurrentChar()) || getCurrentChar() == '_' || getCurrentChar() == '-') {
            value += getCurrentChar();
            advance();
        }
    }
    
    return Token(TokenType::IDENTIFIER, value, startLine, startColumn);
}

Token PropertyExpressionLexer::parseDynamicReference() {
    std::string value;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    // 解析 {{变量}}
    value += getCurrentChar(); // {
    advance();
    value += getCurrentChar(); // {
    advance();
    
    while (position_ < input_.length() && !(getCurrentChar() == '}' && peekNextChar() == '}')) {
        value += getCurrentChar();
        advance();
    }
    
    if (getCurrentChar() == '}' && peekNextChar() == '}') {
        value += getCurrentChar(); // }
        advance();
        value += getCurrentChar(); // }
        advance();
    }
    
    // 解析 ->属性
    if (getCurrentChar() == '-' && peekNextChar() == '>') {
        value += getCurrentChar(); // -
        advance();
        value += getCurrentChar(); // >
        advance();
        
        while (isAlphaNumeric(getCurrentChar()) || getCurrentChar() == '_' || getCurrentChar() == '-') {
            value += getCurrentChar();
            advance();
        }
    }
    
    return Token(TokenType::IDENTIFIER, value, startLine, startColumn);
}

bool PropertyExpressionLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool PropertyExpressionLexer::isAlpha(char c) const {
    return std::isalpha(c);
}

bool PropertyExpressionLexer::isAlphaNumeric(char c) const {
    return std::isalnum(c);
}

bool PropertyExpressionLexer::isWhitespace(char c) const {
    return std::isspace(c);
}

bool PropertyExpressionLexer::isOperatorChar(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '<' || c == '>' || c == '=' || c == '!' || c == '?' ||
           c == ':' || c == '&' || c == '|' || c == '.' || c == '#';
}

} // namespace CHTL