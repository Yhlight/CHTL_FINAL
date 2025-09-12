#include "CHTLJSLexer.h"
#include <iostream>
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLJSLexer::CHTLJSLexer() : position(0), line(1), column(1) {
    initializeKeywords();
    initializeOperators();
}

CHTLJSLexer::~CHTLJSLexer() {}

void CHTLJSLexer::setSource(const std::string& source) {
    this->source = source;
    reset();
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;
    reset();
    
    while (!isEOF()) {
        skipWhitespace();
        if (isEOF()) break;
        
        auto token = nextToken();
        if (token) {
            tokens.push_back(*token);
        }
    }
    
    // 添加EOF token
    tokens.emplace_back(CHTLJSTokenType::EOF_TOKEN, "", line, column, position);
    
    return tokens;
}

std::unique_ptr<CHTLJSToken> CHTLJSLexer::nextToken() {
    if (isEOF()) {
        return createToken(CHTLJSTokenType::EOF_TOKEN, "");
    }
    
    char c = currentChar();
    
    // 处理增强选择器 {{选择器}}
    if (c == '{' && nextChar() == '{') {
        std::string selector = readEnhancedSelector();
        return createToken(CHTLJSTokenType::ENHANCED_SELECTOR, selector);
    }
    
    // 处理响应式值 $变量名$
    if (c == '$') {
        std::string value = readReactiveValue();
        return createToken(CHTLJSTokenType::REACTIVE_VALUE, value);
    }
    
    // 处理字符串
    if (c == '"' || c == '\'') {
        std::string str = readString();
        return createToken(CHTLJSTokenType::STRING, str);
    }
    
    // 处理数字
    if (isDigit(c)) {
        std::string num = readNumber();
        return createToken(CHTLJSTokenType::NUMBER, num);
    }
    
    // 处理标识符和关键字
    if (isAlpha(c) || c == '_') {
        std::string identifier = readIdentifier();
        auto it = keywords.find(identifier);
        if (it != keywords.end()) {
            return createToken(it->second, identifier);
        }
        return createToken(CHTLJSTokenType::IDENTIFIER, identifier);
    }
    
    // 处理操作符
    if (isOperatorChar(c)) {
        std::string op = readOperator();
        auto it = operators.find(op);
        if (it != operators.end()) {
            return createToken(it->second, op);
        }
        return createToken(CHTLJSTokenType::UNKNOWN, op);
    }
    
    // 处理分隔符
    switch (c) {
        case '{':
            advance();
            return createToken(CHTLJSTokenType::LEFT_BRACE, "{");
        case '}':
            advance();
            return createToken(CHTLJSTokenType::RIGHT_BRACE, "}");
        case '(':
            advance();
            return createToken(CHTLJSTokenType::LEFT_PAREN, "(");
        case ')':
            advance();
            return createToken(CHTLJSTokenType::RIGHT_PAREN, ")");
        case '[':
            advance();
            return createToken(CHTLJSTokenType::LEFT_BRACKET, "[");
        case ']':
            advance();
            return createToken(CHTLJSTokenType::RIGHT_BRACKET, "]");
        case ';':
            advance();
            return createToken(CHTLJSTokenType::SEMICOLON, ";");
        case ',':
            advance();
            return createToken(CHTLJSTokenType::COMMA, ",");
        case '.':
            advance();
            return createToken(CHTLJSTokenType::DOT, ".");
        case '\n':
            advance();
            line++;
            column = 1;
            return createToken(CHTLJSTokenType::NEWLINE, "\n");
    }
    
    // 未知字符
    advance();
    return createToken(CHTLJSTokenType::UNKNOWN, std::string(1, c));
}

void CHTLJSLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
}

size_t CHTLJSLexer::getCurrentPosition() const {
    return position;
}

size_t CHTLJSLexer::getCurrentLine() const {
    return line;
}

size_t CHTLJSLexer::getCurrentColumn() const {
    return column;
}

void CHTLJSLexer::initializeKeywords() {
    // CHTL JS关键字
    keywords["vir"] = CHTLJSTokenType::VIR;
    keywords["listen"] = CHTLJSTokenType::LISTEN;
    keywords["animate"] = CHTLJSTokenType::ANIMATE;
    keywords["fileloader"] = CHTLJSTokenType::FILELOADER;
    keywords["router"] = CHTLJSTokenType::ROUTER;
    keywords["iNeverAway"] = CHTLJSTokenType::INEVERAWAY;
    keywords["printMylove"] = CHTLJSTokenType::PRINTMYLOVE;
    keywords["util"] = CHTLJSTokenType::UTIL;
    keywords["then"] = CHTLJSTokenType::THEN;
    keywords["change"] = CHTLJSTokenType::CHANGE;
    keywords["delegate"] = CHTLJSTokenType::DELEGATE;
    keywords["target"] = CHTLJSTokenType::TARGET;
    keywords["duration"] = CHTLJSTokenType::DURATION;
    keywords["easing"] = CHTLJSTokenType::EASING;
    keywords["begin"] = CHTLJSTokenType::BEGIN;
    keywords["when"] = CHTLJSTokenType::WHEN;
    keywords["at"] = CHTLJSTokenType::AT;
    keywords["end"] = CHTLJSTokenType::END;
    keywords["loop"] = CHTLJSTokenType::LOOP;
    keywords["direction"] = CHTLJSTokenType::DIRECTION;
    keywords["delay"] = CHTLJSTokenType::DELAY;
    keywords["callback"] = CHTLJSTokenType::CALLBACK;
}

void CHTLJSLexer::initializeOperators() {
    // 单字符操作符
    operators[":"] = CHTLJSTokenType::COLON;
    operators["="] = CHTLJSTokenType::EQUAL;
    operators["?"] = CHTLJSTokenType::QUESTION;
    operators["!"] = CHTLJSTokenType::EXCLAMATION;
    operators["&"] = CHTLJSTokenType::AMPERSAND;
    operators["|"] = CHTLJSTokenType::PIPE;
    operators["+"] = CHTLJSTokenType::PLUS;
    operators["-"] = CHTLJSTokenType::MINUS;
    operators["*"] = CHTLJSTokenType::MULTIPLY;
    operators["/"] = CHTLJSTokenType::DIVIDE;
    operators["%"] = CHTLJSTokenType::MODULO;
    operators["<"] = CHTLJSTokenType::LESS;
    operators[">"] = CHTLJSTokenType::GREATER;
    
    // 双字符操作符
    operators["->"] = CHTLJSTokenType::ARROW;
    operators["**"] = CHTLJSTokenType::POWER;
    operators["<="] = CHTLJSTokenType::LESS_EQUAL;
    operators[">="] = CHTLJSTokenType::GREATER_EQUAL;
    operators["=="] = CHTLJSTokenType::EQUAL_EQUAL;
    operators["!="] = CHTLJSTokenType::NOT_EQUAL;
    operators["&&"] = CHTLJSTokenType::AND;
    operators["||"] = CHTLJSTokenType::OR;
}

void CHTLJSLexer::skipWhitespace() {
    while (!isEOF() && isWhitespace(currentChar()) && currentChar() != '\n') {
        advance();
    }
}

std::string CHTLJSLexer::readIdentifier() {
    std::string result;
    while (!isEOF() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        result += currentChar();
        advance();
    }
    return result;
}

std::string CHTLJSLexer::readString() {
    std::string result;
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    while (!isEOF() && currentChar() != quote) {
        if (currentChar() == '\\' && nextChar() == quote) {
            advance(); // 跳过反斜杠
            result += currentChar();
            advance();
        } else {
            result += currentChar();
            advance();
        }
    }
    
    if (!isEOF()) {
        advance(); // 跳过结束引号
    }
    
    return result;
}

std::string CHTLJSLexer::readLiteral() {
    std::string result;
    while (!isEOF() && !isWhitespace(currentChar()) && 
           currentChar() != ';' && currentChar() != ':' && 
           currentChar() != '=' && currentChar() != '{' && 
           currentChar() != '}' && currentChar() != '\n') {
        result += currentChar();
        advance();
    }
    return result;
}

std::string CHTLJSLexer::readNumber() {
    std::string result;
    while (!isEOF() && (isDigit(currentChar()) || currentChar() == '.')) {
        result += currentChar();
        advance();
    }
    return result;
}

std::string CHTLJSLexer::readOperator() {
    std::string result;
    char c = currentChar();
    char next = nextChar();
    
    // 检查双字符操作符
    if ((c == '=' && next == '=') || (c == '!' && next == '=') ||
        (c == '<' && next == '=') || (c == '>' && next == '=') ||
        (c == '&' && next == '&') || (c == '|' && next == '|') ||
        (c == '*' && next == '*') || (c == '-' && next == '>')) {
        result += c;
        advance();
        result += currentChar();
        advance();
    } else {
        result += c;
        advance();
    }
    
    return result;
}

std::string CHTLJSLexer::readEnhancedSelector() {
    std::string result;
    result += currentChar(); // '{'
    advance();
    result += currentChar(); // '{'
    advance();
    
    while (!isEOF() && !(currentChar() == '}' && nextChar() == '}')) {
        result += currentChar();
        advance();
    }
    
    if (!isEOF()) {
        result += currentChar(); // '}'
        advance();
        result += currentChar(); // '}'
        advance();
    }
    
    return result;
}

std::string CHTLJSLexer::readReactiveValue() {
    std::string result;
    result += currentChar(); // '$'
    advance();
    
    while (!isEOF() && currentChar() != '$') {
        result += currentChar();
        advance();
    }
    
    if (!isEOF()) {
        result += currentChar(); // '$'
        advance();
    }
    
    return result;
}

bool CHTLJSLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLJSLexer::isOperatorChar(char c) const {
    return c == ':' || c == '=' || c == '?' || c == '!' || c == '&' || 
           c == '|' || c == '+' || c == '-' || c == '*' || c == '/' || 
           c == '%' || c == '<' || c == '>';
}

char CHTLJSLexer::currentChar() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char CHTLJSLexer::nextChar() const {
    if (position + 1 >= source.length()) {
        return '\0';
    }
    return source[position + 1];
}

void CHTLJSLexer::advance() {
    if (position < source.length()) {
        position++;
        column++;
    }
}

void CHTLJSLexer::retreat() {
    if (position > 0) {
        position--;
        column--;
    }
}

bool CHTLJSLexer::isEOF() const {
    return position >= source.length();
}

std::unique_ptr<CHTLJSToken> CHTLJSLexer::createToken(CHTLJSTokenType type, const std::string& value) {
    return std::make_unique<CHTLJSToken>(type, value, line, column, position);
}

// CHTLJSToken类方法实现
bool CHTLJSToken::isKeyword() const {
    return type >= CHTLJSTokenType::VIR && type <= CHTLJSTokenType::CALLBACK;
}

bool CHTLJSToken::isOperator() const {
    return type >= CHTLJSTokenType::COLON && type <= CHTLJSTokenType::OR;
}

bool CHTLJSToken::isDelimiter() const {
    return type >= CHTLJSTokenType::LEFT_BRACE && type <= CHTLJSTokenType::ARROW;
}

std::string CHTLJSToken::toString() const {
    std::string typeStr;
    switch (type) {
        case CHTLJSTokenType::IDENTIFIER: typeStr = "IDENTIFIER"; break;
        case CHTLJSTokenType::STRING: typeStr = "STRING"; break;
        case CHTLJSTokenType::NUMBER: typeStr = "NUMBER"; break;
        case CHTLJSTokenType::LITERAL: typeStr = "LITERAL"; break;
        case CHTLJSTokenType::LEFT_BRACE: typeStr = "LEFT_BRACE"; break;
        case CHTLJSTokenType::RIGHT_BRACE: typeStr = "RIGHT_BRACE"; break;
        case CHTLJSTokenType::COLON: typeStr = "COLON"; break;
        case CHTLJSTokenType::EQUAL: typeStr = "EQUAL"; break;
        case CHTLJSTokenType::ARROW: typeStr = "ARROW"; break;
        case CHTLJSTokenType::VIR: typeStr = "VIR"; break;
        case CHTLJSTokenType::LISTEN: typeStr = "LISTEN"; break;
        case CHTLJSTokenType::ANIMATE: typeStr = "ANIMATE"; break;
        case CHTLJSTokenType::ENHANCED_SELECTOR: typeStr = "ENHANCED_SELECTOR"; break;
        case CHTLJSTokenType::REACTIVE_VALUE: typeStr = "REACTIVE_VALUE"; break;
        case CHTLJSTokenType::EOF_TOKEN: typeStr = "EOF"; break;
        default: typeStr = "UNKNOWN"; break;
    }
    
    return "CHTLJSToken(" + typeStr + ", \"" + value + "\", " + 
           std::to_string(line) + ":" + std::to_string(column) + ")";
}

} // namespace CHTL