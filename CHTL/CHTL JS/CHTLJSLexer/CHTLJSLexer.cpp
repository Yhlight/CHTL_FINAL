#include "CHTLJSLexer.h"
#include <cctype>
#include <sstream>

namespace CHTL {

CHTLJSLexer::CHTLJSLexer(std::string source) 
    : source(std::move(source)), position(0), line(1), column(1) {}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    tokens.clear();
    
    while (position < source.length()) {
        skipWhitespace();
        
        if (position >= source.length()) {
            break;
        }
        
        char c = currentChar();
        
        if (isStringDelimiter(c)) {
            tokens.push_back(scanString());
            continue;
        }
        
        if (isDigit(c)) {
            tokens.push_back(scanNumber());
            continue;
        }
        
        if (isSelectorStart()) {
            tokens.push_back(scanSelector());
            continue;
        }
        
        if (isBooleanStart()) {
            tokens.push_back(scanBoolean());
            continue;
        }
        
        if (isIdentifierStart(c)) {
            tokens.push_back(scanIdentifier());
            continue;
        }
        
        if (isOperatorChar(c) || isDelimiterChar(c)) {
            tokens.push_back(scanOperator());
            continue;
        }
        
        throwError("Unexpected character: " + std::string(1, c));
    }
    
    tokens.emplace_back(CHTLJSTokenType::EOF_TOKEN, "", line, column, position);
    return tokens;
}

char CHTLJSLexer::currentChar() const {
    if (position < source.length()) {
        return source[position];
    }
    return '\0';
}

char CHTLJSLexer::peekChar() const {
    if (position + 1 < source.length()) {
        return source[position + 1];
    }
    return '\0';
}

void CHTLJSLexer::advance() {
    if (position < source.length()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void CHTLJSLexer::skipWhitespace() {
    while (position < source.length() && isWhitespace(currentChar())) {
        advance();
    }
}

void CHTLJSLexer::throwError(const std::string& message) const {
    throw std::runtime_error("CHTL JS Lexing Error at Line " + std::to_string(line) + 
                            ", Column " + std::to_string(column) + ": " + message);
}

CHTLJSToken CHTLJSLexer::scanString() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char delimiter = currentChar();
    advance(); // 跳过开始引号
    
    std::string value;
    while (position < source.length() && currentChar() != delimiter) {
        if (currentChar() == '\\' && position + 1 < source.length()) {
            // 处理转义字符
            advance();
            char escaped = currentChar();
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
            value += currentChar();
        }
        advance();
    }
    
    if (position >= source.length()) {
        throwError("Unterminated string literal");
    }
    
    advance(); // 跳过结束引号
    return CHTLJSToken(CHTLJSTokenType::STRING, value, startLine, startColumn, start);
}

CHTLJSToken CHTLJSLexer::scanNumber() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    bool hasDecimal = false;
    
    while (position < source.length() && (isDigit(currentChar()) || currentChar() == '.')) {
        if (currentChar() == '.') {
            if (hasDecimal) {
                throwError("Invalid number literal: multiple decimal points");
            }
            hasDecimal = true;
        }
        value += currentChar();
        advance();
    }
    
    return CHTLJSToken(CHTLJSTokenType::NUMBER, value, startLine, startColumn, start);
}

CHTLJSToken CHTLJSLexer::scanIdentifier() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    while (position < source.length() && isIdentifierChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    // 检查是否为关键字
    if (CHTLJSToken::isKeyword(value)) {
        CHTLJSTokenType type = CHTLJSToken::getKeywordType(value);
        return CHTLJSToken(type, value, startLine, startColumn, start);
    }
    
    return CHTLJSToken(CHTLJSTokenType::IDENTIFIER, value, startLine, startColumn, start);
}

CHTLJSToken CHTLJSLexer::scanOperator() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char first = currentChar();
    advance();
    
    std::string value(1, first);
    
    // 检查双字符运算符
    if (position < source.length()) {
        char second = currentChar();
        std::string twoChar = value + second;
        
        if (CHTLJSToken::isOperator(twoChar)) {
            advance();
            value = twoChar;
        }
    }
    
    // 检查三字符运算符（如 ->）
    if (position < source.length() && value == "-") {
        char second = currentChar();
        if (second == '>') {
            advance();
            value = "->";
        }
    }
    
    CHTLJSTokenType type = CHTLJSToken::getOperatorType(value);
    if (type == CHTLJSTokenType::ERROR) {
        type = CHTLJSToken::getDelimiterType(value);
    }
    
    return CHTLJSToken(type, value, startLine, startColumn, start);
}

CHTLJSToken CHTLJSLexer::scanSelector() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    // 跳过 {{
    advance();
    advance();
    
    std::string value;
    while (position < source.length() && !(currentChar() == '}' && peekChar() == '}')) {
        value += currentChar();
        advance();
    }
    
    if (position >= source.length()) {
        throwError("Unterminated selector");
    }
    
    // 跳过 }}
    advance();
    advance();
    
    return CHTLJSToken(CHTLJSTokenType::SELECTOR, value, startLine, startColumn, start);
}

CHTLJSToken CHTLJSLexer::scanBoolean() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    while (position < source.length() && isIdentifierChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    if (value == "true" || value == "false" || value == "null") {
        return CHTLJSToken(CHTLJSTokenType::BOOLEAN, value, startLine, startColumn, start);
    }
    
    // 如果不是布尔值，回退并作为标识符处理
    position = start;
    line = startLine;
    column = startColumn;
    return scanIdentifier();
}

bool CHTLJSLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::isAlphanumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return std::isspace(c);
}

bool CHTLJSLexer::isNewline(char c) const {
    return c == '\n' || c == '\r';
}

bool CHTLJSLexer::isStringDelimiter(char c) const {
    return c == '"' || c == '\'';
}

bool CHTLJSLexer::isSelectorStart() const {
    return currentChar() == '{' && peekChar() == '{';
}

bool CHTLJSLexer::isBooleanStart() const {
    char c = currentChar();
    return c == 't' || c == 'f' || c == 'n';
}

bool CHTLJSLexer::isOperatorChar(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '&' || c == '|' || c == '!' || c == '>' || c == '<' ||
           c == '=' || c == '?';
}

bool CHTLJSLexer::isDelimiterChar(char c) const {
    return c == '{' || c == '}' || c == '[' || c == ']' ||
           c == '(' || c == ')' || c == ';' || c == ',' ||
           c == '.' || c == ':';
}

bool CHTLJSLexer::isIdentifierStart(char c) const {
    return isAlpha(c);
}

bool CHTLJSLexer::isIdentifierChar(char c) const {
    return isAlphanumeric(c);
}

} // namespace CHTL