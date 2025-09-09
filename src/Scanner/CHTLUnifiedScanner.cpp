#include "Scanner/CHTLUnifiedScanner.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner()
    : m_position(0)
    , m_line(1)
    , m_column(1)
    , m_strictMode(false)
    , m_preserveComments(true)
    , m_placeholderCounter(0)
{
    initializeKeywordTables();
}

void CHTLUnifiedScanner::initializeKeywordTables() {
    // CHTL关键字
    m_chtlKeywords = {
        {"text", true}, {"style", true}, {"script", true},
        {"[Template]", true}, {"[Custom]", true}, {"[Origin]", true},
        {"[Import]", true}, {"[Namespace]", true}, {"[Configuration]", true},
        {"use", true}, {"inherit", true}, {"delete", true}, {"insert", true},
        {"from", true}, {"as", true}, {"except", true}
    };
    
    // CHTL JS关键字
    m_chtljsKeywords = {
        {"vir", true}, {"listen", true}, {"delegate", true}, {"animate", true},
        {"router", true}, {"fileloader", true}, {"iNeverAway", true},
        {"util", true}, {"change", true}, {"then", true}
    };
    
    // CSS关键字
    m_cssKeywords = {
        {"width", true}, {"height", true}, {"color", true}, {"background", true},
        {"margin", true}, {"padding", true}, {"border", true}, {"display", true},
        {"position", true}, {"float", true}, {"clear", true}, {"overflow", true}
    };
    
    // JS关键字
    m_jsKeywords = {
        {"function", true}, {"var", true}, {"let", true}, {"const", true},
        {"if", true}, {"else", true}, {"for", true}, {"while", true},
        {"return", true}, {"break", true}, {"continue", true}, {"try", true},
        {"catch", true}, {"finally", true}, {"throw", true}, {"new", true},
        {"this", true}, {"typeof", true}, {"instanceof", true}
    };
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan(const std::string& input) {
    m_input = input;
    m_position = 0;
    m_line = 1;
    m_column = 1;
    m_errors.clear();
    m_placeholders.clear();
    m_placeholderCounter = 0;
    
    std::vector<CodeFragment> fragments;
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        CodeFragment fragment = determineFragmentType();
        if (fragment.type != CodeFragmentType::UNKNOWN) {
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scanFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot open file: " + filePath);
        return {};
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return scan(buffer.str());
}

void CHTLUnifiedScanner::setScanOptions(bool strictMode, bool preserveComments) {
    m_strictMode = strictMode;
    m_preserveComments = preserveComments;
}

const std::vector<std::string>& CHTLUnifiedScanner::getErrors() const {
    return m_errors;
}

bool CHTLUnifiedScanner::hasErrors() const {
    return !m_errors.empty();
}

void CHTLUnifiedScanner::reset() {
    m_position = 0;
    m_line = 1;
    m_column = 1;
    m_errors.clear();
    m_placeholders.clear();
    m_placeholderCounter = 0;
}

CodeFragment CHTLUnifiedScanner::determineFragmentType() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    // 检查是否是CHTL语法
    if (isCHTLBoundary()) {
        return scanCHTL();
    }
    
    // 检查是否是CHTL JS语法
    if (isCHTLJSBoundary()) {
        return scanCHTLJS();
    }
    
    // 检查是否是CSS语法
    if (isCSSBoundary()) {
        return scanCSS();
    }
    
    // 检查是否是HTML语法
    if (isHTMLBoundary()) {
        return scanHTML();
    }
    
    // 默认为JavaScript
    return scanJavaScript();
}

CodeFragment CHTLUnifiedScanner::scanCHTL() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    std::string content;
    
    while (!isAtEnd()) {
        char c = currentChar();
        
        // 检查是否是CHTL JS边界
        if (isCHTLJSBoundary()) {
            break;
        }
        
        // 检查是否是CSS边界
        if (isCSSBoundary()) {
            break;
        }
        
        // 检查是否是HTML边界
        if (isHTMLBoundary()) {
            break;
        }
        
        // 检查是否是JS边界
        if (isJSBoundary()) {
            break;
        }
        
        content += c;
        advance();
    }
    
    return CodeFragment(CodeFragmentType::CHTL, content, startLine, 
                       m_line, startColumn, m_column, content);
}

CodeFragment CHTLUnifiedScanner::scanCHTLJS() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    std::string content;
    
    while (!isAtEnd()) {
        char c = currentChar();
        
        // 检查是否是CHTL边界
        if (isCHTLBoundary()) {
            break;
        }
        
        // 检查是否是CSS边界
        if (isCSSBoundary()) {
            break;
        }
        
        // 检查是否是HTML边界
        if (isHTMLBoundary()) {
            break;
        }
        
        // 检查是否是JS边界
        if (isJSBoundary()) {
            break;
        }
        
        content += c;
        advance();
    }
    
    return CodeFragment(CodeFragmentType::CHTL_JS, content, startLine, 
                       m_line, startColumn, m_column, content);
}

CodeFragment CHTLUnifiedScanner::scanCSS() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    std::string content;
    
    while (!isAtEnd()) {
        char c = currentChar();
        
        // 检查是否是CHTL边界
        if (isCHTLBoundary()) {
            break;
        }
        
        // 检查是否是CHTL JS边界
        if (isCHTLJSBoundary()) {
            break;
        }
        
        // 检查是否是HTML边界
        if (isHTMLBoundary()) {
            break;
        }
        
        // 检查是否是JS边界
        if (isJSBoundary()) {
            break;
        }
        
        content += c;
        advance();
    }
    
    return CodeFragment(CodeFragmentType::CSS, content, startLine, 
                       m_line, startColumn, m_column, content);
}

CodeFragment CHTLUnifiedScanner::scanJavaScript() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    std::string content;
    
    while (!isAtEnd()) {
        char c = currentChar();
        
        // 检查是否是CHTL边界
        if (isCHTLBoundary()) {
            break;
        }
        
        // 检查是否是CHTL JS边界
        if (isCHTLJSBoundary()) {
            break;
        }
        
        // 检查是否是CSS边界
        if (isCSSBoundary()) {
            break;
        }
        
        // 检查是否是HTML边界
        if (isHTMLBoundary()) {
            break;
        }
        
        content += c;
        advance();
    }
    
    return CodeFragment(CodeFragmentType::JAVASCRIPT, content, startLine, 
                       m_line, startColumn, m_column, content);
}

CodeFragment CHTLUnifiedScanner::scanHTML() {
    size_t startPos = m_position;
    size_t startLine = m_line;
    size_t startColumn = m_column;
    std::string content;
    
    while (!isAtEnd()) {
        char c = currentChar();
        
        // 检查是否是CHTL边界
        if (isCHTLBoundary()) {
            break;
        }
        
        // 检查是否是CHTL JS边界
        if (isCHTLJSBoundary()) {
            break;
        }
        
        // 检查是否是CSS边界
        if (isCSSBoundary()) {
            break;
        }
        
        // 检查是否是JS边界
        if (isJSBoundary()) {
            break;
        }
        
        content += c;
        advance();
    }
    
    return CodeFragment(CodeFragmentType::HTML, content, startLine, 
                       m_line, startColumn, m_column, content);
}

bool CHTLUnifiedScanner::isAtEnd() const {
    return m_position >= m_input.length();
}

char CHTLUnifiedScanner::currentChar() const {
    if (isAtEnd()) return '\0';
    return m_input[m_position];
}

char CHTLUnifiedScanner::peekChar() const {
    if (m_position + 1 >= m_input.length()) return '\0';
    return m_input[m_position + 1];
}

char CHTLUnifiedScanner::peekChar(int offset) const {
    if (m_position + offset >= m_input.length()) return '\0';
    return m_input[m_position + offset];
}

void CHTLUnifiedScanner::advance() {
    if (!isAtEnd()) {
        if (currentChar() == '\n') {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        m_position++;
    }
}

void CHTLUnifiedScanner::advance(int count) {
    for (int i = 0; i < count && !isAtEnd(); ++i) {
        advance();
    }
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(currentChar())) {
        advance();
    }
}

void CHTLUnifiedScanner::skipLineComment() {
    while (!isAtEnd() && currentChar() != '\n') {
        advance();
    }
}

void CHTLUnifiedScanner::skipBlockComment() {
    advance(); // 跳过 /*
    advance();
    
    while (!isAtEnd()) {
        if (currentChar() == '*' && peekChar() == '/') {
            advance();
            advance();
            break;
        }
        advance();
    }
}

void CHTLUnifiedScanner::skipGeneratorComment() {
    advance(); // 跳过 --
    advance();
    
    while (!isAtEnd() && currentChar() != '\n') {
        advance();
    }
}

bool CHTLUnifiedScanner::isCHTLKeyword(const std::string& word) const {
    return m_chtlKeywords.find(word) != m_chtlKeywords.end();
}

bool CHTLUnifiedScanner::isCHTLJSKeyword(const std::string& word) const {
    return m_chtljsKeywords.find(word) != m_chtljsKeywords.end();
}

bool CHTLUnifiedScanner::isCSSKeyword(const std::string& word) const {
    return m_cssKeywords.find(word) != m_cssKeywords.end();
}

bool CHTLUnifiedScanner::isJSKeyword(const std::string& word) const {
    return m_jsKeywords.find(word) != m_jsKeywords.end();
}

bool CHTLUnifiedScanner::isCHTLBoundary() const {
    // 检查是否是CHTL语法边界
    std::string lookahead = m_input.substr(m_position, std::min(size_t(20), m_input.length() - m_position));
    
    // 检查CHTL关键字
    for (const auto& keyword : m_chtlKeywords) {
        if (lookahead.find(keyword.first) == 0) {
            return true;
        }
    }
    
    // 检查HTML标签
    if (lookahead[0] == '<' && std::isalpha(lookahead[1])) {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCHTLJSBoundary() const {
    // 检查是否是CHTL JS语法边界
    std::string lookahead = m_input.substr(m_position, std::min(size_t(20), m_input.length() - m_position));
    
    // 检查CHTL JS关键字
    for (const auto& keyword : m_chtljsKeywords) {
        if (lookahead.find(keyword.first) == 0) {
            return true;
        }
    }
    
    // 检查增强选择器 {{ }}
    if (lookahead.find("{{") == 0) {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCSSBoundary() const {
    // 检查是否是CSS语法边界
    std::string lookahead = m_input.substr(m_position, std::min(size_t(20), m_input.length() - m_position));
    
    // 检查CSS关键字
    for (const auto& keyword : m_cssKeywords) {
        if (lookahead.find(keyword.first) == 0) {
            return true;
        }
    }
    
    // 检查CSS选择器
    if (lookahead[0] == '.' || lookahead[0] == '#' || lookahead[0] == '@') {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::isJSBoundary() const {
    // 检查是否是JavaScript语法边界
    std::string lookahead = m_input.substr(m_position, std::min(size_t(20), m_input.length() - m_position));
    
    // 检查JS关键字
    for (const auto& keyword : m_jsKeywords) {
        if (lookahead.find(keyword.first) == 0) {
            return true;
        }
    }
    
    // 检查JS函数声明
    if (lookahead.find("function") == 0) {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::isHTMLBoundary() const {
    // 检查是否是HTML语法边界
    std::string lookahead = m_input.substr(m_position, std::min(size_t(20), m_input.length() - m_position));
    
    // 检查HTML标签
    if (lookahead[0] == '<' && std::isalpha(lookahead[1])) {
        return true;
    }
    
    return false;
}

std::string CHTLUnifiedScanner::createPlaceholder(const std::string& content) {
    std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(m_placeholderCounter++);
    m_placeholders[placeholder] = content;
    return placeholder;
}

std::string CHTLUnifiedScanner::restorePlaceholder(const std::string& placeholder) {
    auto it = m_placeholders.find(placeholder);
    if (it != m_placeholders.end()) {
        return it->second;
    }
    return placeholder;
}

void CHTLUnifiedScanner::processPlaceholders(std::string& content) {
    for (const auto& placeholder : m_placeholders) {
        size_t pos = 0;
        while ((pos = content.find(placeholder.first, pos)) != std::string::npos) {
            content.replace(pos, placeholder.first.length(), placeholder.second);
            pos += placeholder.second.length();
        }
    }
}

std::string CHTLUnifiedScanner::readString(char delimiter) {
    advance(); // 跳过开始引号
    std::string result;
    
    while (!isAtEnd() && currentChar() != delimiter) {
        if (currentChar() == '\\' && peekChar() == delimiter) {
            advance(); // 跳过转义字符
        }
        result += currentChar();
        advance();
    }
    
    if (!isAtEnd()) {
        advance(); // 跳过结束引号
    }
    
    return result;
}

std::string CHTLUnifiedScanner::readTemplateString() {
    std::string result;
    
    while (!isAtEnd() && currentChar() != '`') {
        if (currentChar() == '\\' && peekChar() == '`') {
            advance(); // 跳过转义字符
        }
        result += currentChar();
        advance();
    }
    
    if (!isAtEnd()) {
        advance(); // 跳过结束反引号
    }
    
    return result;
}

std::string CHTLUnifiedScanner::readRegex() {
    std::string result;
    
    while (!isAtEnd() && currentChar() != '/') {
        if (currentChar() == '\\' && peekChar() == '/') {
            advance(); // 跳过转义字符
        }
        result += currentChar();
        advance();
    }
    
    if (!isAtEnd()) {
        advance(); // 跳过结束斜杠
    }
    
    return result;
}

void CHTLUnifiedScanner::skipComments() {
    while (!isAtEnd()) {
        if (isLineComment()) {
            skipLineComment();
        } else if (isBlockComment()) {
            skipBlockComment();
        } else if (isGeneratorComment()) {
            skipGeneratorComment();
        } else {
            break;
        }
    }
}

bool CHTLUnifiedScanner::isCommentStart() const {
    return isLineComment() || isBlockComment() || isGeneratorComment();
}

bool CHTLUnifiedScanner::isLineComment() const {
    return currentChar() == '/' && peekChar() == '/';
}

bool CHTLUnifiedScanner::isBlockComment() const {
    return currentChar() == '/' && peekChar() == '*';
}

bool CHTLUnifiedScanner::isGeneratorComment() const {
    return currentChar() == '-' && peekChar() == '-';
}

void CHTLUnifiedScanner::addError(const std::string& message) {
    m_errors.push_back(message);
}

void CHTLUnifiedScanner::addErrorAtPosition(const std::string& message, size_t line, size_t column) {
    std::string errorMsg = "Error at line " + std::to_string(line) + 
                          ", column " + std::to_string(column) + ": " + message;
    addError(errorMsg);
}

void CHTLUnifiedScanner::updatePosition() {
    if (currentChar() == '\n') {
        m_line++;
        m_column = 1;
    } else {
        m_column++;
    }
}

size_t CHTLUnifiedScanner::getCurrentLine() const {
    return m_line;
}

size_t CHTLUnifiedScanner::getCurrentColumn() const {
    return m_column;
}

bool CHTLUnifiedScanner::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLUnifiedScanner::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLUnifiedScanner::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLUnifiedScanner::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool CHTLUnifiedScanner::isOperator(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '!' || c == '<' || c == '>' || c == '&' || c == '|';
}

bool CHTLUnifiedScanner::isPunctuation(char c) const {
    return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
           c == ';' || c == ',' || c == '.' || c == ':' || c == '?';
}

} // namespace CHTL