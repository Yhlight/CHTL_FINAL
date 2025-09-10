#include "CHTLJSLexer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace CHTL_JS {

CHTLJSLexer::CHTLJSLexer() 
    : position(0), line(1), column(1), debugMode(false), strictMode(false), isUnifiedMode(false) {
    initializeKeywords();
    initializeOperators();
    initializeSingleCharTokens();
    initializeMultiCharTokens();
    initializeSyntaxBoundaries();
}

void CHTLJSLexer::setSource(const std::string& source) {
    this->source = source;
    reset();
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;
    reset();
    
    if (isUnifiedMode) {
        return tokenizeFragment(source, "mixed");
    }
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        CHTLJSToken token = getNextToken();
        if (token.getType() != CHTLJSTokenType::UNKNOWN) {
            tokens.push_back(token);
        }
    }
    
    tokens.push_back(CHTLJSToken(CHTLJSTokenType::END_OF_FILE, "", line, column, position));
    return tokens;
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenizeFragment(const std::string& fragment, const std::string& type) {
    std::string originalSource = source;
    source = fragment;
    fragmentType = type;
    reset();
    
    std::vector<CHTLJSToken> tokens = tokenize();
    
    source = originalSource;
    return tokens;
}

CHTLJSToken CHTLJSLexer::getNextToken() {
    if (isAtEnd()) {
        return CHTLJSToken(CHTLJSTokenType::END_OF_FILE, "", line, column, position);
    }
    
    char current = getCurrentChar();
    
    // 跳过空白字符
    if (isWhitespace(current)) {
        skipWhitespace();
        return getNextToken();
    }
    
    // 注释处理
    if (current == '/' && peekChar() == '/') {
        return readSingleLineComment();
    }
    if (current == '/' && peekChar() == '*') {
        return readMultiLineComment();
    }
    if (current == '-' && peekChar() == '-') {
        return readGeneratorComment();
    }
    
    // 字符串处理
    if (isQuote(current)) {
        return readString(current);
    }
    
    // 模板字符串处理
    if (current == '`') {
        return readTemplateLiteral();
    }
    
    // 数字处理
    if (isDigit(current)) {
        return readNumber();
    }
    
    // 增强选择器处理
    if (isEnhancedSelectorStart(source, position)) {
        return readEnhancedSelector();
    }
    
    // 虚对象处理
    if (isVirtualObjectStart(source, position)) {
        return readVirtualObject();
    }
    
    // CHTL JS关键字处理
    if (isCHTLJSSyntax(source, position)) {
        return readCHTLJSKeyword();
    }
    
    // 无修饰字面量处理（CHTL JS特有）
    if (isUnquotedLiteralStart(current)) {
        return readUnquotedLiteral();
    }
    
    // 标识符处理
    if (isLetter(current) || current == '_' || current == '$') {
        return readIdentifier();
    }
    
    // 运算符处理
    if (isOperatorChar(current)) {
        return readOperator();
    }
    
    // 标点符号处理
    if (isPunctuationChar(current)) {
        return readPunctuation();
    }
    
    // 未知字符
    addError("未知字符: " + std::string(1, current));
    advance();
    return CHTLJSToken(CHTLJSTokenType::ERROR, std::string(1, current), line, column, position - 1);
}

// 无修饰字面量处理
CHTLJSToken CHTLJSLexer::readUnquotedLiteral() {
    size_t start = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    
    while (!isAtEnd() && isUnquotedLiteralChar(getCurrentChar())) {
        value += getCurrentChar();
        advance();
    }
    
    return CHTLJSToken(CHTLJSTokenType::LITERAL, value, startLine, startColumn, start);
}

bool CHTLJSLexer::isUnquotedLiteralStart(char c) const {
    // 无修饰字面量不能以数字、运算符、标点符号开始
    return !isDigit(c) && !isOperatorChar(c) && !isPunctuationChar(c) && 
           !isWhitespace(c) && c != '"' && c != '\'' && c != '`' && c != '/';
}

bool CHTLJSLexer::isUnquotedLiteralChar(char c) const {
    // 无修饰字面量字符：字母、数字、下划线、连字符、点号等
    return isAlphaNumeric(c) || c == '_' || c == '-' || c == '.' || c == ':' || c == '@';
}

CHTLJSToken CHTLJSLexer::peekToken() {
    return peekToken(1);
}

CHTLJSToken CHTLJSLexer::peekToken(int offset) {
    size_t originalPos = position;
    size_t originalLine = line;
    size_t originalColumn = column;
    
    for (int i = 0; i < offset; i++) {
        if (isAtEnd()) break;
        getNextToken();
    }
    
    CHTLJSToken token = getNextToken();
    
    position = originalPos;
    line = originalLine;
    column = originalColumn;
    
    return token;
}

// 统一扫描器功能实现
std::vector<std::string> CHTLJSLexer::separateCodeFragments(const std::string& source) {
    std::vector<std::string> fragments;
    size_t pos = 0;
    
    while (pos < source.length()) {
        // 检测语法边界
        auto boundaries = detectBoundaries(source.substr(pos));
        if (!boundaries.empty()) {
            auto boundary = boundaries[0];
            size_t startPos = source.find(boundary.startPattern, pos);
            if (startPos != std::string::npos) {
                size_t endPos = source.find(boundary.endPattern, startPos + boundary.startPattern.length());
                if (endPos != std::string::npos) {
                    endPos += boundary.endPattern.length();
                    
                    // 提取片段
                    std::string fragment = source.substr(startPos, endPos - startPos);
                    fragments.push_back(fragment);
                    
                    // 创建占位符
                    std::string placeholder = createPlaceholder(fragment, boundary.type);
                    registerPlaceholder(placeholder, fragment);
                    
                    pos = endPos;
                    continue;
                }
            }
        }
        
        // 如果没有找到边界，继续搜索
        pos++;
    }
    
    return fragments;
}

std::string CHTLJSLexer::createPlaceholder(const std::string& content, const std::string& type) {
    std::string placeholder = generatePlaceholder(type);
    registerPlaceholder(placeholder, content);
    return placeholder;
}

std::string CHTLJSLexer::restoreFromPlaceholder(const std::string& placeholder) {
    return getPlaceholderContent(placeholder);
}

std::string CHTLJSLexer::processUnifiedCode(const std::string& source) {
    std::string processed = source;
    
    // 替换语法边界为占位符
    processed = replaceBoundariesWithPlaceholders(processed);
    
    return processed;
}

// 语法边界识别实现
void CHTLJSLexer::addSyntaxBoundary(const std::string& start, const std::string& end, 
                                   const std::string& type, bool nested, int priority) {
    SyntaxBoundary boundary;
    boundary.startPattern = start;
    boundary.endPattern = end;
    boundary.type = type;
    boundary.isNested = nested;
    boundary.priority = priority;
    std::string upperType = type;
    std::transform(upperType.begin(), upperType.end(), upperType.begin(), ::toupper);
    boundary.placeholder = "_" + upperType + "_PLACEHOLDER_";
    
    boundaries.push_back(boundary);
    boundaryMap[start] = end;
}

std::vector<CHTLJSLexer::SyntaxBoundary> CHTLJSLexer::detectBoundaries(const std::string& code) {
    std::vector<SyntaxBoundary> detected;
    
    for (const auto& boundary : boundaries) {
        if (code.find(boundary.startPattern) != std::string::npos) {
            detected.push_back(boundary);
        }
    }
    
    // 按优先级排序
    std::sort(detected.begin(), detected.end(), 
              [](const SyntaxBoundary& a, const SyntaxBoundary& b) {
                  return a.priority > b.priority;
              });
    
    return detected;
}

std::string CHTLJSLexer::replaceBoundariesWithPlaceholders(const std::string& code) {
    std::string result = code;
    auto detected = detectBoundaries(code);
    
    for (const auto& boundary : detected) {
        size_t pos = 0;
        while ((pos = result.find(boundary.startPattern, pos)) != std::string::npos) {
            size_t endPos = result.find(boundary.endPattern, pos + boundary.startPattern.length());
            if (endPos != std::string::npos) {
                endPos += boundary.endPattern.length();
                std::string content = result.substr(pos, endPos - pos);
                std::string placeholder = createPlaceholder(content, boundary.type);
                result.replace(pos, endPos - pos, placeholder);
                pos += placeholder.length();
            } else {
                break;
            }
        }
    }
    
    return result;
}

// 错误处理实现
void CHTLJSLexer::addError(const std::string& error) {
    errors.push_back(formatError(error, line, column));
}

void CHTLJSLexer::addWarning(const std::string& warning) {
    warnings.push_back(formatError(warning, line, column));
}

void CHTLJSLexer::clearErrors() {
    errors.clear();
}

void CHTLJSLexer::clearWarnings() {
    warnings.clear();
}

void CHTLJSLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    clearErrors();
    clearWarnings();
    clearPlaceholders();
}

void CHTLJSLexer::resetPosition() {
    position = 0;
    line = 1;
    column = 1;
}

bool CHTLJSLexer::isAtEnd() const {
    return position >= source.length();
}

std::string CHTLJSLexer::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTLJSLexer Debug Info:\n";
    oss << "Position: " << position << "\n";
    oss << "Line: " << line << ", Column: " << column << "\n";
    oss << "Source length: " << source.length() << "\n";
    oss << "Debug mode: " << (debugMode ? "ON" : "OFF") << "\n";
    oss << "Strict mode: " << (strictMode ? "ON" : "OFF") << "\n";
    oss << "Unified mode: " << (isUnifiedMode ? "ON" : "OFF") << "\n";
    oss << "Errors: " << errors.size() << "\n";
    oss << "Warnings: " << warnings.size() << "\n";
    return oss.str();
}

std::string CHTLJSLexer::getTokenDebugInfo(const std::vector<CHTLJSToken>& tokens) const {
    std::ostringstream oss;
    oss << "Token Debug Info:\n";
    oss << "Total tokens: " << tokens.size() << "\n";
    
    for (size_t i = 0; i < tokens.size(); i++) {
        oss << "[" << i << "] " << tokens[i].toDebugString() << "\n";
    }
    
    return oss.str();
}

void CHTLJSLexer::printTokens(const std::vector<CHTLJSToken>& tokens) const {
    for (const auto& token : tokens) {
        std::cout << token.toDebugString() << std::endl;
    }
}

// 初始化方法实现
void CHTLJSLexer::initializeKeywords() {
    // 基本关键字（仅CHTL JS需要的）
    keywords["true"] = CHTLJSTokenType::TRUE;
    keywords["false"] = CHTLJSTokenType::FALSE;
    keywords["null"] = CHTLJSTokenType::NULL_VALUE;
    keywords["undefined"] = CHTLJSTokenType::UNDEFINED;
    
    // CHTL JS特有关键字
    keywords["vir"] = CHTLJSTokenType::VIRTUAL_OBJECT;
    keywords["listen"] = CHTLJSTokenType::LISTEN;
    keywords["delegate"] = CHTLJSTokenType::DELEGATE;
    keywords["animate"] = CHTLJSTokenType::ANIMATE;
    keywords["router"] = CHTLJSTokenType::ROUTER;
    keywords["fileloader"] = CHTLJSTokenType::FILELOADER;
    keywords["iNeverAway"] = CHTLJSTokenType::INEVERAWAY;
    keywords["util"] = CHTLJSTokenType::UTIL;
    keywords["then"] = CHTLJSTokenType::THEN;
    keywords["change"] = CHTLJSTokenType::CHANGE;
}

void CHTLJSLexer::initializeOperators() {
    // 基本运算符（仅CHTL JS需要的）
    operators["="] = CHTLJSTokenType::ASSIGN;
    operators["->"] = CHTLJSTokenType::ARROW;
    operators[":"] = CHTLJSTokenType::COLON;
    operators[";"] = CHTLJSTokenType::SEMICOLON;
    operators[","] = CHTLJSTokenType::COMMA;
}

void CHTLJSLexer::initializeSingleCharTokens() {
    singleCharTokens['('] = CHTLJSTokenType::LEFT_PAREN;
    singleCharTokens[')'] = CHTLJSTokenType::RIGHT_PAREN;
    singleCharTokens['['] = CHTLJSTokenType::LEFT_BRACKET;
    singleCharTokens[']'] = CHTLJSTokenType::RIGHT_BRACKET;
    singleCharTokens['{'] = CHTLJSTokenType::LEFT_BRACE;
    singleCharTokens['}'] = CHTLJSTokenType::RIGHT_BRACE;
    singleCharTokens[';'] = CHTLJSTokenType::SEMICOLON;
    singleCharTokens[','] = CHTLJSTokenType::COMMA;
    singleCharTokens[':'] = CHTLJSTokenType::COLON;
}

void CHTLJSLexer::initializeMultiCharTokens() {
    // 多字符运算符已在initializeOperators中处理
}

void CHTLJSLexer::initializeSyntaxBoundaries() {
    // CHTL JS语法边界
    addSyntaxBoundary("{{", "}}", "enhanced_selector", true, 10);
    addSyntaxBoundary("vir ", " {", "virtual_object", true, 9);
    addSyntaxBoundary("listen ", " {", "listen", true, 8);
    addSyntaxBoundary("delegate ", " {", "delegate", true, 8);
    addSyntaxBoundary("animate ", " {", "animate", true, 8);
    addSyntaxBoundary("router ", " {", "router", true, 8);
    addSyntaxBoundary("fileloader ", " {", "fileloader", true, 8);
    addSyntaxBoundary("iNeverAway ", " {", "iNeverAway", true, 8);
    addSyntaxBoundary("util ", " ->", "util", true, 8);
    addSyntaxBoundary("change ", " {", "change", true, 8);
}

// 字符处理实现
char CHTLJSLexer::getCurrentChar() const {
    return position < source.length() ? source[position] : '\0';
}

char CHTLJSLexer::getNextChar() {
    if (position >= source.length()) return '\0';
    char c = source[position++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

char CHTLJSLexer::peekChar() const {
    return peekChar(1);
}

char CHTLJSLexer::peekChar(int offset) const {
    size_t pos = position + offset;
    return pos < source.length() ? source[pos] : '\0';
}

void CHTLJSLexer::advance() {
    advance(1);
}

void CHTLJSLexer::advance(int count) {
    for (int i = 0; i < count; i++) {
        getNextChar();
    }
}

void CHTLJSLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(getCurrentChar())) {
        advance();
    }
}

void CHTLJSLexer::skipLine() {
    while (!isAtEnd() && !isNewline(getCurrentChar())) {
        advance();
    }
    if (!isAtEnd()) advance(); // 跳过换行符
}

void CHTLJSLexer::skipBlockComment() {
    while (!isAtEnd() && !(getCurrentChar() == '*' && peekChar() == '/')) {
        advance();
    }
    if (!isAtEnd()) advance(2); // 跳过 */
}

// 类型判断实现
bool CHTLJSLexer::isLetter(char c) const {
    return std::isalpha(c) || c == '_' || c == '$';
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isLetter(c) || isDigit(c);
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return std::isspace(c);
}

bool CHTLJSLexer::isNewline(char c) const {
    return c == '\n' || c == '\r';
}

bool CHTLJSLexer::isQuote(char c) const {
    return c == '"' || c == '\'';
}

bool CHTLJSLexer::isOperatorChar(char c) const {
    return operators.find(std::string(1, c)) != operators.end() ||
           c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '!' || c == '<' || c == '>' || c == '&' ||
           c == '|' || c == '^' || c == '~' || c == '?';
}

bool CHTLJSLexer::isPunctuationChar(char c) const {
    return singleCharTokens.find(c) != singleCharTokens.end();
}

bool CHTLJSLexer::isKeyword(const std::string& word) const {
    return keywords.find(word) != keywords.end();
}

bool CHTLJSLexer::isOperator(const std::string& op) const {
    return operators.find(op) != operators.end();
}

bool CHTLJSLexer::isSingleCharToken(char c) const {
    return singleCharTokens.find(c) != singleCharTokens.end();
}

bool CHTLJSLexer::isMultiCharToken(const std::string& str) const {
    return operators.find(str) != operators.end();
}

// 词法分析辅助实现
CHTLJSToken CHTLJSLexer::readIdentifier() {
    size_t start = position;
    while (!isAtEnd() && isAlphaNumeric(getCurrentChar())) {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    CHTLJSTokenType type = isKeyword(value) ? keywords.at(value) : CHTLJSTokenType::IDENTIFIER;
    
    return CHTLJSToken(type, value, line, column - value.length(), start);
}

CHTLJSToken CHTLJSLexer::readNumber() {
    size_t start = position;
    bool hasDecimal = false;
    bool hasExponent = false;
    
    while (!isAtEnd() && (isDigit(getCurrentChar()) || getCurrentChar() == '.' || 
                         getCurrentChar() == 'e' || getCurrentChar() == 'E' ||
                         getCurrentChar() == '+' || getCurrentChar() == '-')) {
        char c = getCurrentChar();
        if (c == '.') {
            if (hasDecimal) break;
            hasDecimal = true;
        } else if (c == 'e' || c == 'E') {
            if (hasExponent) break;
            hasExponent = true;
            advance();
            if (getCurrentChar() == '+' || getCurrentChar() == '-') {
                advance();
            }
            continue;
        }
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    return CHTLJSToken(CHTLJSTokenType::NUMBER, value, line, column - value.length(), start);
}

CHTLJSToken CHTLJSLexer::readString(char delimiter) {
    size_t start = position;
    advance(); // 跳过开始引号
    
    while (!isAtEnd() && getCurrentChar() != delimiter) {
        if (getCurrentChar() == '\\') {
            advance(); // 跳过转义字符
        }
        advance();
    }
    
    if (!isAtEnd()) {
        advance(); // 跳过结束引号
    }
    
    std::string value = source.substr(start, position - start);
    return CHTLJSToken(CHTLJSTokenType::STRING, value, line, column - value.length(), start);
}

CHTLJSToken CHTLJSLexer::readString() {
    return readString('"');
}

CHTLJSToken CHTLJSLexer::readTemplateLiteral() {
    size_t start = position;
    advance(); // 跳过开始反引号
    
    while (!isAtEnd() && getCurrentChar() != '`') {
        if (getCurrentChar() == '\\') {
            advance(); // 跳过转义字符
        }
        advance();
    }
    
    if (!isAtEnd()) {
        advance(); // 跳过结束反引号
    }
    
    std::string value = source.substr(start, position - start);
    return CHTLJSToken(CHTLJSTokenType::STRING, value, line, column - value.length(), start);
}

CHTLJSToken CHTLJSLexer::readComment() {
    if (getCurrentChar() == '/' && peekChar() == '/') {
        return readSingleLineComment();
    } else if (getCurrentChar() == '/' && peekChar() == '*') {
        return readMultiLineComment();
    } else if (getCurrentChar() == '-' && peekChar() == '-') {
        return readGeneratorComment();
    }
    
    return CHTLJSToken(CHTLJSTokenType::ERROR, "", line, column, position);
}

CHTLJSToken CHTLJSLexer::readSingleLineComment() {
    size_t start = position;
    skipLine();
    
    std::string value = source.substr(start, position - start);
    return CHTLJSToken(CHTLJSTokenType::SINGLE_COMMENT, value, line, column - value.length(), start);
}

CHTLJSToken CHTLJSLexer::readMultiLineComment() {
    size_t start = position;
    advance(2); // 跳过 /*
    
    while (!isAtEnd() && !(getCurrentChar() == '*' && peekChar() == '/')) {
        advance();
    }
    
    if (!isAtEnd()) {
        advance(2); // 跳过 */
    }
    
    std::string value = source.substr(start, position - start);
    return CHTLJSToken(CHTLJSTokenType::MULTI_COMMENT, value, line, column - value.length(), start);
}

CHTLJSToken CHTLJSLexer::readGeneratorComment() {
    size_t start = position;
    advance(2); // 跳过 --
    skipLine();
    
    std::string value = source.substr(start, position - start);
    return CHTLJSToken(CHTLJSTokenType::GENERATOR_COMMENT, value, line, column - value.length(), start);
}

CHTLJSToken CHTLJSLexer::readOperator() {
    size_t start = position;
    char first = getCurrentChar();
    advance();
    
    // 检查双字符运算符
    if (!isAtEnd()) {
        char second = getCurrentChar();
        std::string twoChar = std::string(1, first) + second;
        if (isOperator(twoChar)) {
            advance();
            return CHTLJSToken(operators[twoChar], twoChar, line, column - 2, start);
        }
    }
    
    // 单字符运算符
    std::string oneChar = std::string(1, first);
    if (isOperator(oneChar)) {
        return CHTLJSToken(operators[oneChar], oneChar, line, column - 1, start);
    }
    
    return CHTLJSToken(CHTLJSTokenType::ERROR, oneChar, line, column - 1, start);
}

CHTLJSToken CHTLJSLexer::readPunctuation() {
    char c = getCurrentChar();
    advance();
    
    if (isSingleCharToken(c)) {
        return CHTLJSToken(singleCharTokens[c], std::string(1, c), line, column - 1, position - 1);
    }
    
    return CHTLJSToken(CHTLJSTokenType::ERROR, std::string(1, c), line, column - 1, position - 1);
}

CHTLJSToken CHTLJSLexer::readEnhancedSelector() {
    size_t start = position;
    advance(2); // 跳过 {{
    
    while (!isAtEnd() && !(getCurrentChar() == '}' && peekChar() == '}')) {
        advance();
    }
    
    if (!isAtEnd()) {
        advance(2); // 跳过 }}
    }
    
    std::string value = source.substr(start, position - start);
    return CHTLJSToken(CHTLJSTokenType::ENHANCED_SELECTOR, value, line, column - value.length(), start);
}

CHTLJSToken CHTLJSLexer::readVirtualObject() {
    size_t start = position;
    advance(3); // 跳过 vir
    
    skipWhitespace();
    
    // 读取标识符
    while (!isAtEnd() && isAlphaNumeric(getCurrentChar())) {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    return CHTLJSToken(CHTLJSTokenType::VIRTUAL_OBJECT, value, line, column - value.length(), start);
}

CHTLJSToken CHTLJSLexer::readCHTLJSKeyword() {
    // 这里需要根据具体的CHTL JS关键字进行处理
    return readIdentifier();
}

// 特殊语法处理实现
bool CHTLJSLexer::isCHTLJSSyntax(const std::string& code, size_t pos) const {
    return isEnhancedSelectorStart(code, pos) ||
           isVirtualObjectStart(code, pos) ||
           isListenStart(code, pos) ||
           isDelegateStart(code, pos) ||
           isAnimateStart(code, pos) ||
           isRouterStart(code, pos) ||
           isFileloaderStart(code, pos) ||
           isUtilStart(code, pos);
}

bool CHTLJSLexer::isEnhancedSelectorStart(const std::string& code, size_t pos) const {
    return pos + 1 < code.length() && code[pos] == '{' && code[pos + 1] == '{';
}

bool CHTLJSLexer::isVirtualObjectStart(const std::string& code, size_t pos) const {
    return code.substr(pos, 3) == "vir";
}

bool CHTLJSLexer::isListenStart(const std::string& code, size_t pos) const {
    return code.substr(pos, 6) == "listen";
}

bool CHTLJSLexer::isDelegateStart(const std::string& code, size_t pos) const {
    return code.substr(pos, 8) == "delegate";
}

bool CHTLJSLexer::isAnimateStart(const std::string& code, size_t pos) const {
    return code.substr(pos, 7) == "animate";
}

bool CHTLJSLexer::isRouterStart(const std::string& code, size_t pos) const {
    return code.substr(pos, 6) == "router";
}

bool CHTLJSLexer::isFileloaderStart(const std::string& code, size_t pos) const {
    return code.substr(pos, 10) == "fileloader";
}

bool CHTLJSLexer::isUtilStart(const std::string& code, size_t pos) const {
    return code.substr(pos, 4) == "util";
}

// 占位符管理实现
std::string CHTLJSLexer::generatePlaceholder(const std::string& type) {
    static int counter = 0;
    return "_" + type + "_PLACEHOLDER_" + std::to_string(++counter) + "_";
}

void CHTLJSLexer::registerPlaceholder(const std::string& placeholder, const std::string& content) {
    placeholderMap[placeholder] = content;
    placeholders.push_back(placeholder);
}

std::string CHTLJSLexer::getPlaceholderContent(const std::string& placeholder) {
    auto it = placeholderMap.find(placeholder);
    return it != placeholderMap.end() ? it->second : "";
}

void CHTLJSLexer::clearPlaceholders() {
    placeholders.clear();
    placeholderMap.clear();
}

// 错误处理辅助实现
void CHTLJSLexer::reportError(const std::string& message, size_t line, size_t column) {
    addError(message);
}

void CHTLJSLexer::reportWarning(const std::string& message, size_t line, size_t column) {
    addWarning(message);
}

std::string CHTLJSLexer::formatError(const std::string& message, size_t line, size_t column) const {
    std::ostringstream oss;
    oss << "Error at line " << line << ", column " << column << ": " << message;
    return oss.str();
}

// 其他辅助方法实现
std::string CHTLJSLexer::getContext(size_t pos, int contextSize) const {
    size_t start = pos > contextSize ? pos - contextSize : 0;
    size_t end = pos + contextSize < source.length() ? pos + contextSize : source.length();
    return source.substr(start, end - start);
}

std::string CHTLJSLexer::getLineContext(size_t line) const {
    // 按行分割获取指定行的上下文
    std::stringstream ss(source);
    std::string currentLine;
    size_t currentLineNum = 1;
    
    while (std::getline(ss, currentLine)) {
        if (currentLineNum == line) {
            return currentLine;
        }
        currentLineNum++;
    }
    
    return "";
}

void CHTLJSLexer::printContext(size_t pos) const {
    std::cout << "Context around position " << pos << ": " << getContext(pos) << std::endl;
}

bool CHTLJSLexer::isBoundaryStart(const std::string& code, size_t pos) const {
    for (const auto& boundary : boundaries) {
        if (code.substr(pos, boundary.startPattern.length()) == boundary.startPattern) {
            return true;
        }
    }
    return false;
}

bool CHTLJSLexer::isBoundaryEnd(const std::string& code, size_t pos, const std::string& startPattern) const {
    auto it = boundaryMap.find(startPattern);
    if (it != boundaryMap.end()) {
        return code.substr(pos, it->second.length()) == it->second;
    }
    return false;
}

std::string CHTLJSLexer::findMatchingBoundary(const std::string& code, size_t startPos, const std::string& startPattern) const {
    auto it = boundaryMap.find(startPattern);
    if (it != boundaryMap.end()) {
        size_t pos = code.find(it->second, startPos);
        if (pos != std::string::npos) {
            return code.substr(startPos, pos + it->second.length() - startPos);
        }
    }
    return "";
}

int CHTLJSLexer::calculateBoundaryPriority(const std::string& startPattern) const {
    for (const auto& boundary : boundaries) {
        if (boundary.startPattern == startPattern) {
            return boundary.priority;
        }
    }
    return 0;
}

// 验证方法实现
bool CHTLJSLexer::validateTokens(const std::vector<CHTLJSToken>& tokens) const {
    for (const auto& token : tokens) {
        if (!token.isValid()) {
            return false;
        }
    }
    return true;
}

bool CHTLJSLexer::validateSyntax(const std::vector<CHTLJSToken>& tokens) const {
    // 验证CHTL JS语法
    if (tokens.empty()) {
        return false;
    }
    
    // 检查是否有错误token
    for (const auto& token : tokens) {
        if (token.getType() == CHTLJSTokenType::ERROR) {
            return false;
        }
    }
    
    // 检查是否以END_OF_FILE结束
    return tokens.back().getType() == CHTLJSTokenType::END_OF_FILE;
}

// 优化方法实现
std::vector<CHTLJSToken> CHTLJSLexer::optimizeTokens(const std::vector<CHTLJSToken>& tokens) {
    std::vector<CHTLJSToken> optimized;
    
    for (size_t i = 0; i < tokens.size(); i++) {
        if (i + 1 < tokens.size() && canMergeTokens(tokens[i], tokens[i + 1])) {
            optimized.push_back(mergeTokens(tokens[i], tokens[i + 1]));
            i++; // 跳过下一个token
        } else {
            optimized.push_back(tokens[i]);
        }
    }
    
    return optimized;
}

std::vector<CHTLJSToken> CHTLJSLexer::compressTokens(const std::vector<CHTLJSToken>& tokens) {
    std::vector<CHTLJSToken> compressed;
    
    for (const auto& token : tokens) {
        if (canCompressToken(token)) {
            compressed.push_back(compressToken(token));
        } else {
            compressed.push_back(token);
        }
    }
    
    return compressed;
}

std::vector<CHTLJSToken> CHTLJSLexer::decompressTokens(const std::vector<CHTLJSToken>& tokens) {
    std::vector<CHTLJSToken> decompressed;
    
    for (const auto& token : tokens) {
        decompressed.push_back(decompressToken(token));
    }
    
    return decompressed;
}

// 优化辅助方法实现
bool CHTLJSLexer::canMergeTokens(const CHTLJSToken& token1, const CHTLJSToken& token2) const {
    // CHTL JS中某些token可以合并，如连续的字符串字面量
    return (token1.getType() == CHTLJSTokenType::STRING && 
            token2.getType() == CHTLJSTokenType::STRING) ||
           (token1.getType() == CHTLJSTokenType::LITERAL && 
            token2.getType() == CHTLJSTokenType::LITERAL);
}

CHTLJSToken CHTLJSLexer::mergeTokens(const CHTLJSToken& token1, const CHTLJSToken& token2) const {
    if (canMergeTokens(token1, token2)) {
        std::string mergedValue = token1.getValue() + token2.getValue();
        return CHTLJSToken(token1.getType(), mergedValue, token1.getLine(), token1.getColumn(), token1.getPosition());
    }
    return token1;
}

bool CHTLJSLexer::canCompressToken(const CHTLJSToken& token) const {
    return token.getType() == CHTLJSTokenType::SINGLE_COMMENT; // 使用 SINGLE_COMMENT 代替 WHITESPACE
}

CHTLJSToken CHTLJSLexer::compressToken(const CHTLJSToken& token) const {
    if (!canCompressToken(token)) {
        return token;
    }
    
    // 压缩空白字符
    std::string compressed = token.getValue();
    compressed = std::regex_replace(compressed, std::regex(R"(\s+)"), " ");
    compressed = std::regex_replace(compressed, std::regex(R"(^\s+|\s+$)"), "");
    
    return CHTLJSToken(token.getType(), compressed, token.getLine(), token.getColumn(), token.getPosition());
}

CHTLJSToken CHTLJSLexer::decompressToken(const CHTLJSToken& token) const {
    // 解压缩是压缩的逆操作，这里简化处理
    return token;
}

// 其他未实现的方法
bool CHTLJSLexer::isDeclarationSyntax(const std::string& code, size_t pos) const { return false; }
CHTLJSToken CHTLJSLexer::readDeclaration(const std::string& keyword) { return CHTLJSToken(); }
std::vector<CHTLJSToken> CHTLJSLexer::parseKeyValuePairs(const std::string& content) { return {}; }
std::vector<CHTLJSToken> CHTLJSLexer::parseUnorderedPairs(const std::string& content) { return {}; }
std::vector<CHTLJSToken> CHTLJSLexer::parseOptionalPairs(const std::string& content) { return {}; }
bool CHTLJSLexer::isUnquotedLiteral(const std::string& code, size_t pos) const { return false; }
bool CHTLJSLexer::isValidUnquotedLiteral(const std::string& literal) const { return false; }
bool CHTLJSLexer::isChainSyntax(const std::string& code, size_t pos) const { return false; }
CHTLJSToken CHTLJSLexer::readChainExpression() { return CHTLJSToken(); }
std::vector<CHTLJSToken> CHTLJSLexer::parseChainElements(const std::string& chain) { return {}; }
std::string CHTLJSLexer::extractCHTLFragment(const std::string& source, size_t& pos) { return ""; }
std::string CHTLJSLexer::extractCHTLJSFragment(const std::string& source, size_t& pos) { return ""; }
std::string CHTLJSLexer::extractJSFragment(const std::string& source, size_t& pos) { return ""; }
std::string CHTLJSLexer::extractCSSFragment(const std::string& source, size_t& pos) { return ""; }
std::string CHTLJSLexer::extractMixedFragment(const std::string& source, size_t& pos) { return ""; }
bool CHTLJSLexer::isValidIdentifier(const std::string& identifier) const { return true; }
bool CHTLJSLexer::isValidNumber(const std::string& number) const { return true; }
bool CHTLJSLexer::isValidString(const std::string& str) const { return true; }
bool CHTLJSLexer::isValidOperator(const std::string& op) const { return true; }
bool CHTLJSLexer::isValidPunctuation(const std::string& punct) const { return true; }

} // namespace CHTL_JS