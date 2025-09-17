#include "CHTLJSLexer.h"
#include <iostream>
#include <cctype>
#include <sstream>

namespace CHTLJS {

CHTLJSLexer::CHTLJSLexer() 
    : currentPos(0), currentLine(1), currentColumn(1), 
      debugMode(false), minifyOutput(false) {}

CHTLJSLexer::CHTLJSLexer(const std::string& source) 
    : source(source), currentPos(0), currentLine(1), currentColumn(1),
      debugMode(false), minifyOutput(false) {}

char CHTLJSLexer::getCurrentChar() const {
    if (currentPos >= source.length()) {
        return '\0';
    }
    return source[currentPos];
}

char CHTLJSLexer::peekChar(size_t offset) const {
    size_t pos = currentPos + offset;
    if (pos >= source.length()) {
        return '\0';
    }
    return source[pos];
}

void CHTLJSLexer::advance() {
    if (currentPos < source.length()) {
        if (source[currentPos] == '\n') {
            currentLine++;
            currentColumn = 1;
        } else {
            currentColumn++;
        }
        currentPos++;
    }
}

void CHTLJSLexer::skipWhitespace() {
    while (currentPos < source.length() && isWhitespace(getCurrentChar())) {
        advance();
    }
}

void CHTLJSLexer::skipComment() {
    if (getCurrentChar() == '/' && peekChar() == '/') {
        // 单行注释
        while (currentPos < source.length() && !isNewline(getCurrentChar())) {
            advance();
        }
    } else if (getCurrentChar() == '/' && peekChar() == '*') {
        // 多行注释
        advance(); // 跳过 /
        advance(); // 跳过 *
        while (currentPos < source.length()) {
            if (getCurrentChar() == '*' && peekChar() == '/') {
                advance(); // 跳过 *
                advance(); // 跳过 /
                break;
            }
            advance();
        }
    }
}

Token CHTLJSLexer::scanIdentifier() {
    std::string value;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPos = currentPos;
    
    while (currentPos < source.length() && isAlphaNumeric(getCurrentChar())) {
        value += getCurrentChar();
        advance();
    }
    
    // 检查是否是CHTL JS关键字
    if (isVirKeyword(value)) {
        return Token(TokenType::VIR, value, startLine, startColumn, startPos);
    } else if (isListenKeyword(value)) {
        return Token(TokenType::LISTEN, value, startLine, startColumn, startPos);
    } else if (isAnimateKeyword(value)) {
        return Token(TokenType::ANIMATE, value, startLine, startColumn, startPos);
    } else if (isDelegateKeyword(value)) {
        return Token(TokenType::DELEGATE, value, startLine, startColumn, startPos);
    } else if (isScriptLoaderKeyword(value)) {
        return Token(TokenType::SCRIPT_LOADER, value, startLine, startColumn, startPos);
    }
    
    return Token(TokenType::IDENTIFIER, value, startLine, startColumn, startPos);
}

Token CHTLJSLexer::scanString() {
    std::string value;
    char quote = getCurrentChar();
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPos = currentPos;
    
    advance(); // 跳过开始引号
    
    while (currentPos < source.length() && getCurrentChar() != quote) {
        if (getCurrentChar() == '\\') {
            advance(); // 跳过反斜杠
            if (currentPos < source.length()) {
                char escaped = getCurrentChar();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += escaped; break;
                }
                advance();
            }
        } else {
            value += getCurrentChar();
            advance();
        }
    }
    
    if (currentPos < source.length()) {
        advance(); // 跳过结束引号
    }
    
    return Token(TokenType::STRING, value, startLine, startColumn, startPos);
}

Token CHTLJSLexer::scanNumber() {
    std::string value;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPos = currentPos;
    
    // 处理整数部分
    while (currentPos < source.length() && isDigit(getCurrentChar())) {
        value += getCurrentChar();
        advance();
    }
    
    // 处理小数部分
    if (getCurrentChar() == '.' && isDigit(peekChar())) {
        value += getCurrentChar();
        advance();
        while (currentPos < source.length() && isDigit(getCurrentChar())) {
            value += getCurrentChar();
            advance();
        }
    }
    
    // 处理科学计数法
    if (getCurrentChar() == 'e' || getCurrentChar() == 'E') {
        value += getCurrentChar();
        advance();
        if (getCurrentChar() == '+' || getCurrentChar() == '-') {
            value += getCurrentChar();
            advance();
        }
        while (currentPos < source.length() && isDigit(getCurrentChar())) {
            value += getCurrentChar();
            advance();
        }
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn, startPos);
}

Token CHTLJSLexer::scanOperator() {
    char first = getCurrentChar();
    char second = peekChar();
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPos = currentPos;
    
    std::string value;
    value += first;
    
    // 双字符操作符
    if (second != '\0') {
        std::string twoChar = value + second;
        if (twoChar == "==" || twoChar == "!=" || twoChar == "<=" || 
            twoChar == ">=" || twoChar == "&&" || twoChar == "||" ||
            twoChar == "++" || twoChar == "--" || twoChar == "**") {
            advance();
            advance();
            return Token(TokenType::EQUAL, twoChar, startLine, startColumn, startPos);
        }
    }
    
    advance();
    
    switch (first) {
        case '=': return Token(TokenType::ASSIGN, value, startLine, startColumn, startPos);
        case '+': return Token(TokenType::PLUS, value, startLine, startColumn, startPos);
        case '-': return Token(TokenType::MINUS, value, startLine, startColumn, startPos);
        case '*': return Token(TokenType::MULTIPLY, value, startLine, startColumn, startPos);
        case '/': return Token(TokenType::DIVIDE, value, startLine, startColumn, startPos);
        case '%': return Token(TokenType::MODULO, value, startLine, startColumn, startPos);
        case '<': return Token(TokenType::LESS, value, startLine, startColumn, startPos);
        case '>': return Token(TokenType::GREATER, value, startLine, startColumn, startPos);
        case '!': return Token(TokenType::NOT, value, startLine, startColumn, startPos);
        case '&': return Token(TokenType::BITWISE_AND, value, startLine, startColumn, startPos);
        case '|': return Token(TokenType::BITWISE_OR, value, startLine, startColumn, startPos);
        case '^': return Token(TokenType::BITWISE_XOR, value, startLine, startColumn, startPos);
        case '~': return Token(TokenType::BITWISE_NOT, value, startLine, startColumn, startPos);
        default: return Token(TokenType::UNKNOWN, value, startLine, startColumn, startPos);
    }
}

Token CHTLJSLexer::scanPunctuation() {
    char c = getCurrentChar();
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPos = currentPos;
    
    advance();
    
    switch (c) {
        case ';': return Token(TokenType::SEMICOLON, ";", startLine, startColumn, startPos);
        case ',': return Token(TokenType::COMMA, ",", startLine, startColumn, startPos);
        case '.': return Token(TokenType::DOT, ".", startLine, startColumn, startPos);
        case ':': return Token(TokenType::COLON, ":", startLine, startColumn, startPos);
        case '?': return Token(TokenType::QUESTION, "?", startLine, startColumn, startPos);
        case '(': return Token(TokenType::LEFT_PAREN, "(", startLine, startColumn, startPos);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", startLine, startColumn, startPos);
        case '[': return Token(TokenType::LEFT_BRACKET, "[", startLine, startColumn, startPos);
        case ']': return Token(TokenType::RIGHT_BRACKET, "]", startLine, startColumn, startPos);
        case '{': return Token(TokenType::LEFT_BRACE, "{", startLine, startColumn, startPos);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", startLine, startColumn, startPos);
        case '@': return Token(TokenType::AT, "@", startLine, startColumn, startPos);
        case '#': return Token(TokenType::HASH, "#", startLine, startColumn, startPos);
        case '$': return Token(TokenType::DOLLAR, "$", startLine, startColumn, startPos);
        case '_': return Token(TokenType::UNDERSCORE, "_", startLine, startColumn, startPos);
        case '~': return Token(TokenType::TILDE, "~", startLine, startColumn, startPos);
        case '|': return Token(TokenType::PIPE, "|", startLine, startColumn, startPos);
        case '&': return Token(TokenType::AMPERSAND, "&", startLine, startColumn, startPos);
        default: return Token(TokenType::UNKNOWN, std::string(1, c), startLine, startColumn, startPos);
    }
}

Token CHTLJSLexer::scanEnhancedSelector() {
    std::string value;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPos = currentPos;
    
    // 跳过 {{
    advance();
    advance();
    
    while (currentPos < source.length() && !(getCurrentChar() == '}' && peekChar() == '}')) {
        value += getCurrentChar();
        advance();
    }
    
    // 跳过 }}
    if (currentPos < source.length()) {
        advance();
        advance();
    }
    
    return Token(TokenType::SELECTOR_START, value, startLine, startColumn, startPos);
}

Token CHTLJSLexer::scanResponsiveValue() {
    std::string value;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPos = currentPos;
    
    // 跳过 $
    advance();
    
    while (currentPos < source.length() && getCurrentChar() != '$') {
        value += getCurrentChar();
        advance();
    }
    
    // 跳过结束 $
    if (currentPos < source.length()) {
        advance();
    }
    
    return Token(TokenType::REACTIVE_START, value, startLine, startColumn, startPos);
}

Token CHTLJSLexer::scanEventBinding() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPos = currentPos;
    
    // 跳过 &->
    advance();
    advance();
    advance();
    
    return Token(TokenType::EVENT_BINDING, "&->", startLine, startColumn, startPos);
}

Token CHTLJSLexer::scanArrowAccess() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPos = currentPos;
    
    // 跳过 ->
    advance();
    advance();
    
    return Token(TokenType::ARROW, "->", startLine, startColumn, startPos);
}

bool CHTLJSLexer::isVirKeyword(const std::string& text) const {
    return text == "Vir" || text == "vir";
}

bool CHTLJSLexer::isListenKeyword(const std::string& text) const {
    return text == "Listen" || text == "listen";
}

bool CHTLJSLexer::isAnimateKeyword(const std::string& text) const {
    return text == "Animate" || text == "animate";
}

bool CHTLJSLexer::isDelegateKeyword(const std::string& text) const {
    return text == "Delegate" || text == "delegate";
}

bool CHTLJSLexer::isScriptLoaderKeyword(const std::string& text) const {
    return text == "ScriptLoader" || text == "scriptloader";
}

bool CHTLJSLexer::isEnhancedSelectorStart() const {
    return getCurrentChar() == '{' && peekChar() == '{';
}

bool CHTLJSLexer::isResponsiveValueStart() const {
    return getCurrentChar() == '$';
}

bool CHTLJSLexer::isEventBindingStart() const {
    return getCurrentChar() == '&' && peekChar() == '-' && peekChar(2) == '>';
}

bool CHTLJSLexer::isArrowAccessStart() const {
    return getCurrentChar() == '-' && peekChar() == '>';
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

bool CHTLJSLexer::isNewline(char c) const {
    return c == '\n';
}

bool CHTLJSLexer::isQuote(char c) const {
    return c == '"' || c == '\'';
}

bool CHTLJSLexer::isOperatorChar(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '!' || c == '<' || c == '>' || c == '&' ||
           c == '|' || c == '^' || c == '~';
}

bool CHTLJSLexer::isPunctuationChar(char c) const {
    return c == ';' || c == ',' || c == '.' || c == ':' || c == '?' ||
           c == '(' || c == ')' || c == '[' || c == ']' || c == '{' ||
           c == '}' || c == '@' || c == '#' || c == '$' || c == '_' ||
           c == '~' || c == '|' || c == '&';
}

void CHTLJSLexer::addError(const std::string& message) {
    std::ostringstream oss;
    oss << "Lexer error at line " << currentLine 
        << ", column " << currentColumn << ": " << message;
    errors.push_back(oss.str());
}

void CHTLJSLexer::logDebug(const std::string& message) const {
    if (debugMode) {
        std::cout << "[CHTLJSLexer] " << message << std::endl;
    }
}

TokenList CHTLJSLexer::tokenize() {
    TokenList tokens;
    reset();
    
    while (currentPos < source.length()) {
        char c = getCurrentChar();
        
        if (isWhitespace(c)) {
            if (!minifyOutput) {
                tokens.addToken(TokenType::WHITESPACE, " ", currentLine, currentColumn, currentPos);
            }
            skipWhitespace();
        } else if (isNewline(c)) {
            if (!minifyOutput) {
                tokens.addToken(TokenType::NEWLINE, "\n", currentLine, currentColumn, currentPos);
            }
            advance();
        } else if (c == '/' && (peekChar() == '/' || peekChar() == '*')) {
            skipComment();
        } else if (isAlpha(c)) {
            tokens.addToken(scanIdentifier());
        } else if (isDigit(c)) {
            tokens.addToken(scanNumber());
        } else if (isQuote(c)) {
            tokens.addToken(scanString());
        } else if (isEnhancedSelectorStart()) {
            tokens.addToken(scanEnhancedSelector());
        } else if (isResponsiveValueStart()) {
            tokens.addToken(scanResponsiveValue());
        } else if (isEventBindingStart()) {
            tokens.addToken(scanEventBinding());
        } else if (isArrowAccessStart()) {
            tokens.addToken(scanArrowAccess());
        } else if (isOperatorChar(c)) {
            tokens.addToken(scanOperator());
        } else if (isPunctuationChar(c)) {
            tokens.addToken(scanPunctuation());
        } else {
            addError("Unknown character: " + std::string(1, c));
            advance();
        }
    }
    
    // 添加EOF token
    tokens.addToken(TokenType::EOF_TOKEN, "", currentLine, currentColumn, currentPos);
    
    if (debugMode) {
        printTokens();
        printStatistics();
    }
    
    return tokens;
}

TokenList CHTLJSLexer::tokenize(const std::string& source) {
    setSource(source);
    return tokenize();
}

void CHTLJSLexer::setDebugMode(bool debug) {
    debugMode = debug;
}

bool CHTLJSLexer::isDebugMode() const {
    return debugMode;
}

void CHTLJSLexer::setMinifyOutput(bool minify) {
    minifyOutput = minify;
}

bool CHTLJSLexer::isMinifyOutput() const {
    return minifyOutput;
}

void CHTLJSLexer::setSource(const std::string& source) {
    this->source = source;
    reset();
}

bool CHTLJSLexer::hasError() const {
    return !errors.empty();
}

std::string CHTLJSLexer::getLastError() const {
    if (errors.empty()) {
        return "";
    }
    return errors.back();
}

std::vector<std::string> CHTLJSLexer::getErrors() const {
    return errors;
}

void CHTLJSLexer::reset() {
    currentPos = 0;
    currentLine = 1;
    currentColumn = 1;
    errors.clear();
}

void CHTLJSLexer::clear() {
    source.clear();
    reset();
}

void CHTLJSLexer::printTokens() const {
    std::cout << "=== CHTL JS Tokens ===" << std::endl;
    // 这里需要访问TokenList的tokens，但为了简化，我们直接输出
    std::cout << "Token count: " << source.length() << std::endl;
    std::cout << "=====================" << std::endl;
}

void CHTLJSLexer::printStatistics() const {
    std::cout << "=== CHTL JS Lexer Statistics ===" << std::endl;
    std::cout << "Source length: " << source.length() << " characters" << std::endl;
    std::cout << "Lines: " << currentLine << std::endl;
    std::cout << "Errors: " << errors.size() << std::endl;
    std::cout << "Debug mode: " << (debugMode ? "enabled" : "disabled") << std::endl;
    std::cout << "Minify output: " << (minifyOutput ? "enabled" : "disabled") << std::endl;
    std::cout << "===============================" << std::endl;
}

} // namespace CHTLJS