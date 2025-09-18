#include "CHTLLexer.h"
#include <cctype>
#include <functional>
#include <iostream>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& source)
    : source_(source), currentPos_(0), currentLine_(1), currentColumn_(1), tokenIndex_(0) {
    GlobalMap::initialize();
}

TokenList CHTLLexer::tokenize() {
    tokens_.clear();
    currentPos_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
    tokenIndex_ = 0;
    
    while (hasMoreTokens()) {
        skipWhitespace();
        
        if (!hasMoreTokens()) break;
        
        TokenPtr token = nextToken();
        if (token) {
            tokens_.push_back(token);
        }
    }
    
    // 添加文件结束标记
    tokens_.push_back(std::make_shared<Token>(TokenType::END_OF_FILE, "", currentLine_, currentColumn_));
    
    return tokens_;
}

TokenPtr CHTLLexer::nextToken() {
    if (!hasMoreTokens()) {
        return std::make_shared<Token>(TokenType::END_OF_FILE, "", currentLine_, currentColumn_);
    }
    
    char c = currentChar();
    
    // 处理块关键字 [Template], [Custom], [Origin], [Import], [Configuration], [Namespace]
    if (c == '[') {
        return readBlockKeyword();
    }
    
    // 处理标识符和关键字
    if (isAlpha(c) || c == '_') {
        return readIdentifier();
    }
    
    // 处理数字
    if (isDigit(c)) {
        return readNumber();
    }
    
    // 处理字符串
    if (isStringDelimiter(c)) {
        return readString();
    }
    
    // 处理注释
    if (isCommentStart(c)) {
        return readComment();
    }
    
    // 处理生成器注释
    if (isGeneratorCommentStart()) {
        return readGeneratorComment();
    }
    
    // 处理运算符
    if (isOperator(std::string(1, c)) || (c == '*' && peekChar() == '*') || 
        (c == '&' && peekChar() == '&') || (c == '|' && peekChar() == '|') ||
        (c == '=' && peekChar() == '=') || (c == '!' && peekChar() == '=') ||
        (c == '<' && peekChar() == '=') || (c == '>' && peekChar() == '=') ||
        (c == '-' && peekChar() == '>')) {
        return readOperator();
    }
    
    // 处理符号
    if (c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' ||
        c == ';' || c == ':' || c == '=' || c == ',' || c == '.' || c == '#' ||
        c == '@' || c == '&' || c == '?' || c == '!' || c == '_' || c == '$') {
        return readSymbol();
    }
    
    // 处理字面量（无修饰字符串）
    if (c != ' ' && c != '\t' && c != '\n' && c != '\r' && !isStringDelimiter(c) &&
        !isCommentStart(c) && c != '{' && c != '}' && c != '(' && c != ')' &&
        c != '[' && c != ']' && c != ';' && c != ':' && c != '=' && c != ',' &&
        c != '.' && c != '#' && c != '@' && c != '&' && c != '?' && c != '!' &&
        c != '_' && c != '$' && !isOperator(std::string(1, c))) {
        return readLiteral();
    }
    
    // 未知字符
    reportError("Unknown character: " + std::string(1, c));
    advance();
    return nullptr;
}

TokenPtr CHTLLexer::peekToken(size_t offset) {
    size_t originalPos = currentPos_;
    size_t originalLine = currentLine_;
    size_t originalColumn = currentColumn_;
    size_t originalIndex = tokenIndex_;
    
    TokenPtr token = nullptr;
    for (size_t i = 0; i <= offset; ++i) {
        token = nextToken();
    }
    
    currentPos_ = originalPos;
    currentLine_ = originalLine;
    currentColumn_ = originalColumn;
    tokenIndex_ = originalIndex;
    
    return token;
}

bool CHTLLexer::hasMoreTokens() const {
    return currentPos_ < source_.length();
}

void CHTLLexer::reset() {
    currentPos_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
    tokenIndex_ = 0;
}

char CHTLLexer::currentChar() const {
    return (currentPos_ < source_.length()) ? source_[currentPos_] : '\0';
}

char CHTLLexer::peekChar(size_t offset) const {
    size_t pos = currentPos_ + offset;
    return (pos < source_.length()) ? source_[pos] : '\0';
}

void CHTLLexer::advance() {
    if (currentPos_ < source_.length()) {
        if (source_[currentPos_] == '\n') {
            currentLine_++;
            currentColumn_ = 1;
        } else {
            currentColumn_++;
        }
        currentPos_++;
    }
}

void CHTLLexer::skipWhitespace() {
    while (hasMoreTokens() && isWhitespace(currentChar())) {
        advance();
    }
}

void CHTLLexer::skipComment() {
    while (hasMoreTokens() && currentChar() != '\n') {
        advance();
    }
}

void CHTLLexer::skipMultilineComment() {
    while (hasMoreTokens()) {
        if (currentChar() == '*' && peekChar() == '/') {
            advance(); // 跳过 *
            advance(); // 跳过 /
            break;
        }
        advance();
    }
}

TokenPtr CHTLLexer::readBlockKeyword() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    // 读取 [keyword]
    std::string keyword;
    while (hasMoreTokens() && currentChar() != ']') {
        keyword += currentChar();
        advance();
    }
    
    if (currentChar() == ']') {
        keyword += currentChar();
        advance();
    }
    
    TokenType type = identifyBlockKeyword(keyword);
    return std::make_shared<Token>(type, keyword, startLine, startColumn);
}

TokenPtr CHTLLexer::readIdentifier() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (hasMoreTokens() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        advance();
    }
    
    std::string value = source_.substr(start, currentPos_ - start);
    TokenType type = identifyKeyword(value);
    
    if (type == TokenType::UNKNOWN) {
        type = TokenType::IDENTIFIER;
    }
    
    return std::make_shared<Token>(type, value, startLine, startColumn);
}

TokenPtr CHTLLexer::readString() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    char delimiter = currentChar();
    advance(); // 跳过开始引号
    
    std::string value;
    while (hasMoreTokens() && currentChar() != delimiter) {
        if (currentChar() == '\\' && peekChar() == delimiter) {
            advance(); // 跳过反斜杠
            value += delimiter;
        } else {
            value += currentChar();
        }
        advance();
    }
    
    if (currentChar() == delimiter) {
        advance(); // 跳过结束引号
    } else {
        reportError("Unterminated string literal");
    }
    
    return std::make_shared<Token>(TokenType::STRING, value, startLine, startColumn);
}

TokenPtr CHTLLexer::readNumber() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (hasMoreTokens() && (isDigit(currentChar()) || currentChar() == '.')) {
        advance();
    }
    
    std::string value = source_.substr(start, currentPos_ - start);
    return std::make_shared<Token>(TokenType::NUMBER, value, startLine, startColumn);
}

TokenPtr CHTLLexer::readLiteral() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (hasMoreTokens() && !isWhitespace(currentChar()) && 
           !isStringDelimiter(currentChar()) && !isCommentStart(currentChar()) &&
           currentChar() != '{' && currentChar() != '}' && currentChar() != '(' &&
           currentChar() != ')' && currentChar() != '[' && currentChar() != ']' &&
           currentChar() != ';' && currentChar() != ':' && currentChar() != '=' &&
           currentChar() != ',' && currentChar() != '.' && currentChar() != '#' &&
           currentChar() != '@' && currentChar() != '&' && currentChar() != '?' &&
           currentChar() != '!' && currentChar() != '_' && currentChar() != '$' &&
           !isOperator(std::string(1, currentChar()))) {
        advance();
    }
    
    std::string value = source_.substr(start, currentPos_ - start);
    return std::make_shared<Token>(TokenType::LITERAL, value, startLine, startColumn);
}

TokenPtr CHTLLexer::readOperator() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    char c = currentChar();
    advance();
    
    // 检查双字符运算符
    if (hasMoreTokens()) {
        char next = currentChar();
        std::string twoChar = std::string(1, c) + next;
        
        if (isOperator(twoChar)) {
            advance();
            return std::make_shared<Token>(identifyOperator(twoChar), twoChar, startLine, startColumn);
        }
    }
    
    // 单字符运算符
    std::string singleChar = std::string(1, c);
    return std::make_shared<Token>(identifyOperator(singleChar), singleChar, startLine, startColumn);
}

TokenPtr CHTLLexer::readSymbol() {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    char c = currentChar();
    advance();
    
    TokenType type;
    switch (c) {
        case '{': type = TokenType::LEFT_BRACE; break;
        case '}': type = TokenType::RIGHT_BRACE; break;
        case '(': type = TokenType::LEFT_PAREN; break;
        case ')': type = TokenType::RIGHT_PAREN; break;
        case '[': type = TokenType::LEFT_BRACKET; break;
        case ']': type = TokenType::RIGHT_BRACKET; break;
        case ';': type = TokenType::SEMICOLON; break;
        case ':': type = TokenType::COLON; break;
        case '=': type = TokenType::EQUALS; break;
        case ',': type = TokenType::COMMA; break;
        case '.': type = TokenType::DOT; break;
        case '#': type = TokenType::HASH; break;
        case '@': type = TokenType::AT; break;
        case '&': type = TokenType::AMPERSAND; break;
        case '?': type = TokenType::QUESTION; break;
        case '!': type = TokenType::EXCLAMATION; break;
        case '_': type = TokenType::UNDERSCORE; break;
        case '$': type = TokenType::DOLLAR; break;
        default: type = TokenType::UNKNOWN; break;
    }
    
    return std::make_shared<Token>(type, std::string(1, c), startLine, startColumn);
}

TokenPtr CHTLLexer::readComment() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    if (isMultilineCommentStart()) {
        advance(); // 跳过 /
        advance(); // 跳过 *
        skipMultilineComment();
        std::string value = source_.substr(start, currentPos_ - start);
        return std::make_shared<Token>(TokenType::MULTILINE_COMMENT, value, startLine, startColumn);
    } else {
        skipComment();
        std::string value = source_.substr(start, currentPos_ - start);
        return std::make_shared<Token>(TokenType::COMMENT, value, startLine, startColumn);
    }
}

TokenPtr CHTLLexer::readGeneratorComment() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    advance(); // 跳过 #
    
    // 跳过空格
    while (hasMoreTokens() && isWhitespace(currentChar())) {
        advance();
    }
    
    // 读取注释内容直到行尾
    while (hasMoreTokens() && currentChar() != '\n') {
        advance();
    }
    
    std::string value = source_.substr(start, currentPos_ - start);
    return std::make_shared<Token>(TokenType::GENERATOR_COMMENT, value, startLine, startColumn);
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool CHTLLexer::isStringDelimiter(char c) const {
    return c == '"' || c == '\'';
}

bool CHTLLexer::isCommentStart(char c) const {
    return c == '/';
}

bool CHTLLexer::isMultilineCommentStart() const {
    return currentChar() == '/' && peekChar() == '*';
}

bool CHTLLexer::isGeneratorCommentStart() const {
    return currentChar() == '#';
}

bool CHTLLexer::isBlockKeywordStart() const {
    return currentChar() == '[';
}

TokenType CHTLLexer::identifyKeyword(const std::string& word) {
    return GlobalMap::getKeywordType(word);
}

TokenType CHTLLexer::identifyOperator(const std::string& op) {
    return GlobalMap::getOperatorType(op);
}

TokenType CHTLLexer::identifyBlockKeyword(const std::string& word) {
    return GlobalMap::getKeywordType(word);
}

void CHTLLexer::setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler) {
    errorHandler_ = handler;
}

void CHTLLexer::reportError(const std::string& message) {
    if (errorHandler_) {
        errorHandler_(message, currentLine_, currentColumn_);
    } else {
        std::cerr << "Error at " << currentLine_ << ":" << currentColumn_ << ": " << message << std::endl;
    }
}

} // namespace CHTL