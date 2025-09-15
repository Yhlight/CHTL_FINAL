#include "CHTLJSLexer.h"
#include <iostream>
#include <cctype>
#include <stdexcept>

namespace CHTLJS {

CHTLJSLexer::CHTLJSLexer(const std::string& source) 
    : source(source), position(0), line(1), column(1), start(0) {
    initializeKeywords();
    initializeOperators();
}

void CHTLJSLexer::initializeKeywords() {
    keywords = {
        // CHTL JS 特有关键字
        {"Vir", TokenType::VIR},
        {"Listen", TokenType::LISTEN},
        {"Animate", TokenType::ANIMATE},
        {"Router", TokenType::ROUTER},
        {"ScriptLoader", TokenType::SCRIPT_LOADER},
        {"Delegate", TokenType::DELEGATE},
        
        // JavaScript 关键字
        {"function", TokenType::FUNCTION},
        {"var", TokenType::VAR},
        {"let", TokenType::LET},
        {"const", TokenType::CONST},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"for", TokenType::FOR},
        {"while", TokenType::WHILE},
        {"do", TokenType::DO},
        {"switch", TokenType::SWITCH},
        {"case", TokenType::CASE},
        {"default", TokenType::DEFAULT},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"return", TokenType::RETURN},
        {"try", TokenType::TRY},
        {"catch", TokenType::CATCH},
        {"finally", TokenType::FINALLY},
        {"throw", TokenType::THROW},
        {"new", TokenType::NEW},
        {"this", TokenType::THIS},
        {"super", TokenType::SUPER},
        {"class", TokenType::CLASS},
        {"extends", TokenType::EXTENDS},
        {"implements", TokenType::IMPLEMENTS},
        {"interface", TokenType::INTERFACE},
        {"enum", TokenType::ENUM},
        {"public", TokenType::PUBLIC},
        {"private", TokenType::PRIVATE},
        {"protected", TokenType::PROTECTED},
        {"static", TokenType::STATIC},
        {"abstract", TokenType::ABSTRACT},
        {"final", TokenType::FINAL},
        {"native", TokenType::NATIVE},
        {"synchronized", TokenType::SYNCHRONIZED},
        {"volatile", TokenType::VOLATILE},
        {"transient", TokenType::TRANSIENT},
        {"strictfp", TokenType::STRICTFP},
        
        // 布尔值
        {"true", TokenType::BOOLEAN},
        {"false", TokenType::BOOLEAN},
        {"null", TokenType::BOOLEAN},
        {"undefined", TokenType::BOOLEAN}
    };
}

void CHTLJSLexer::initializeOperators() {
    operators = {
        {"=", TokenType::ASSIGN},
        {":", TokenType::COLON},
        {";", TokenType::SEMICOLON},
        {",", TokenType::COMMA},
        {".", TokenType::DOT},
        {"->", TokenType::ARROW},
        {"?", TokenType::QUESTION},
        {"!", TokenType::EXCLAMATION},
        {"==", TokenType::EQUAL},
        {"!=", TokenType::NOT_EQUAL},
        {"<", TokenType::LESS},
        {">", TokenType::GREATER},
        {"<=", TokenType::LESS_EQUAL},
        {">=", TokenType::GREATER_EQUAL},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"*", TokenType::MULTIPLY},
        {"/", TokenType::DIVIDE},
        {"%", TokenType::MODULO},
        {"**", TokenType::POWER},
        {"&&", TokenType::AND},
        {"||", TokenType::OR},
        {"(", TokenType::LEFT_PAREN},
        {")", TokenType::RIGHT_PAREN},
        {"{", TokenType::LEFT_BRACE},
        {"}", TokenType::RIGHT_BRACE},
        {"[", TokenType::LEFT_BRACKET},
        {"]", TokenType::RIGHT_BRACKET},
        {"@", TokenType::AT},
        {"#", TokenType::HASH},
        {"$", TokenType::DOLLAR},
        {"_", TokenType::UNDERSCORE},
        {"~", TokenType::TILDE},
        {"|", TokenType::PIPE},
        {"&", TokenType::AMPERSAND}
    };
}

char CHTLJSLexer::current() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char CHTLJSLexer::peek(size_t offset) const {
    size_t index = position + offset;
    if (index >= source.length()) {
        return '\0';
    }
    return source[index];
}

void CHTLJSLexer::advance() {
    if (position < source.length()) {
        if (current() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

bool CHTLJSLexer::isAtEnd() const {
    return position >= source.length();
}

void CHTLJSLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(current())) {
        advance();
    }
}

void CHTLJSLexer::skipComment() {
    if (current() == '/' && peek() == '/') {
        skipLineComment();
    } else if (current() == '/' && peek() == '*') {
        skipBlockComment();
    }
}

void CHTLJSLexer::skipLineComment() {
    while (!isAtEnd() && !isNewline(current())) {
        advance();
    }
}

void CHTLJSLexer::skipBlockComment() {
    advance(); // 跳过 '/'
    advance(); // 跳过 '*'
    
    while (!isAtEnd()) {
        if (current() == '*' && peek() == '/') {
            advance(); // 跳过 '*'
            advance(); // 跳过 '/'
            break;
        }
        advance();
    }
}

Token CHTLJSLexer::scanString() {
    char quote = current();
    advance(); // 跳过开始引号
    
    std::string value;
    while (!isAtEnd() && current() != quote) {
        if (current() == '\\') {
            advance(); // 跳过反斜杠
            if (!isAtEnd()) {
                switch (current()) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += current(); break;
                }
                advance();
            }
        } else {
            value += current();
            advance();
        }
    }
    
    if (isAtEnd()) {
        error("Unterminated string");
        return makeToken(TokenType::UNKNOWN);
    }
    
    advance(); // 跳过结束引号
    return makeToken(TokenType::STRING, value);
}

Token CHTLJSLexer::scanNumber() {
    std::string value;
    
    // 处理整数部分
    while (isDigit(current())) {
        value += current();
        advance();
    }
    
    // 处理小数部分
    if (current() == '.' && isDigit(peek())) {
        value += current();
        advance();
        while (isDigit(current())) {
            value += current();
            advance();
        }
    }
    
    // 处理科学计数法
    if (current() == 'e' || current() == 'E') {
        value += current();
        advance();
        if (current() == '+' || current() == '-') {
            value += current();
            advance();
        }
        while (isDigit(current())) {
            value += current();
            advance();
        }
    }
    
    return makeToken(TokenType::NUMBER, value);
}

Token CHTLJSLexer::scanIdentifier() {
    std::string value;
    
    while (isAlphaNumeric(current()) || current() == '_') {
        value += current();
        advance();
    }
    
    // 检查是否是关键字
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return makeToken(it->second, value);
    }
    
    return makeToken(TokenType::IDENTIFIER, value);
}

Token CHTLJSLexer::scanOperator() {
    std::string value;
    value += current();
    advance();
    
    // 检查双字符运算符
    if (!isAtEnd()) {
        std::string twoChar = value + current();
        auto it = operators.find(twoChar);
        if (it != operators.end()) {
            advance();
            return makeToken(it->second, twoChar);
        }
    }
    
    // 检查单字符运算符
    auto it = operators.find(value);
    if (it != operators.end()) {
        return makeToken(it->second, value);
    }
    
    return makeToken(TokenType::UNKNOWN, value);
}

Token CHTLJSLexer::scanSelector() {
    advance(); // 跳过第一个 '{'
    advance(); // 跳过第二个 '{'
    
    std::string value;
    while (!isAtEnd() && !(current() == '}' && peek() == '}')) {
        value += current();
        advance();
    }
    
    if (isAtEnd()) {
        error("Unterminated selector");
        return makeToken(TokenType::UNKNOWN);
    }
    
    advance(); // 跳过第一个 '}'
    advance(); // 跳过第二个 '}'
    
    return makeToken(TokenType::SELECTOR_START, value);
}

Token CHTLJSLexer::scanReactiveValue() {
    advance(); // 跳过第一个 '$'
    
    std::string value;
    while (!isAtEnd() && current() != '$') {
        value += current();
        advance();
    }
    
    if (isAtEnd()) {
        error("Unterminated reactive value");
        return makeToken(TokenType::UNKNOWN);
    }
    
    advance(); // 跳过结束 '$'
    
    return makeToken(TokenType::REACTIVE_START, value);
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLJSLexer::isNewline(char c) const {
    return c == '\n';
}

Token CHTLJSLexer::makeToken(TokenType type, const std::string& value) {
    return Token(type, value, line, column - value.length(), start);
}

Token CHTLJSLexer::makeToken(TokenType type, char value) {
    return Token(type, std::string(1, value), line, column - 1, start);
}

void CHTLJSLexer::error(const std::string& message) const {
    std::ostringstream oss;
    oss << "Lexer error at line " << line << ", column " << column << ": " << message;
    throw std::runtime_error(oss.str());
}

TokenList CHTLJSLexer::tokenize() {
    TokenList tokens;
    position = 0;
    line = 1;
    column = 1;
    
    while (!isAtEnd()) {
        start = position;
        
        skipWhitespace();
        if (isAtEnd()) break;
        
        char c = current();
        
        if (c == '/' && (peek() == '/' || peek() == '*')) {
            skipComment();
            continue;
        }
        
        if (c == '"' || c == '\'') {
            tokens.addToken(scanString());
            continue;
        }
        
        if (isDigit(c)) {
            tokens.addToken(scanNumber());
            continue;
        }
        
        if (isAlpha(c)) {
            tokens.addToken(scanIdentifier());
            continue;
        }
        
        if (c == '{' && peek() == '{') {
            tokens.addToken(scanSelector());
            continue;
        }
        
        if (c == '$') {
            tokens.addToken(scanReactiveValue());
            continue;
        }
        
        if (operators.find(std::string(1, c)) != operators.end()) {
            tokens.addToken(scanOperator());
            continue;
        }
        
        // 未知字符
        advance();
        tokens.addToken(makeToken(TokenType::UNKNOWN, c));
    }
    
    tokens.addToken(Token(TokenType::EOF_TOKEN, "", line, column, position));
    return tokens;
}

TokenList CHTLJSLexer::tokenize(const std::string& source) {
    setSource(source);
    return tokenize();
}

void CHTLJSLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    start = 0;
}

void CHTLJSLexer::setSource(const std::string& source) {
    this->source = source;
    reset();
}

std::string CHTLJSLexer::getSource() const {
    return source;
}

size_t CHTLJSLexer::getPosition() const {
    return position;
}

size_t CHTLJSLexer::getLine() const {
    return line;
}

size_t CHTLJSLexer::getColumn() const {
    return column;
}

} // namespace CHTLJS