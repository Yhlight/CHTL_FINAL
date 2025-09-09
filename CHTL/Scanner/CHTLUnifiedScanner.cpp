#include "CHTLUnifiedScanner.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner() 
    : position(0), line(1), column(1), debugMode(false), strictMode(false), placeholderCounter(0) {
}

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source), position(0), line(1), column(1), debugMode(false), strictMode(false), placeholderCounter(0) {
}

void CHTLUnifiedScanner::setSource(const std::string& source) {
    this->source = source;
    position = 0;
    line = 1;
    column = 1;
    placeholderCounter = 0;
    clearMessages();
}

ScanResult CHTLUnifiedScanner::scan(const std::string& inputFile) {
    try {
        // 读取输入文件
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            addError("无法打开文件: " + inputFile);
            return ScanResult{};
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        setSource(buffer.str());
        file.close();
        
        return scan();
        
    } catch (const std::exception& e) {
        addError("扫描文件时发生错误: " + std::string(e.what()));
        return ScanResult{};
    }
}

ScanResult CHTLUnifiedScanner::scan() {
    ScanResult result;
    result.originalSource = source;
    
    try {
        result = scanSource();
    } catch (const std::exception& e) {
        addError("扫描源代码时发生错误: " + std::string(e.what()));
    }
    
    result.errors = errors;
    result.warnings = warnings;
    
    return result;
}

ScanResult CHTLUnifiedScanner::scanSource() {
    ScanResult result;
    result.originalSource = source;
    
    while (position < source.length()) {
        skipWhitespace();
        
        if (position >= source.length()) {
            break;
        }
        
        // 识别代码类型
        std::string codeType = identifyCodeType();
        
        CodeFragment fragment;
        fragment.startLine = getCurrentLine();
        fragment.startColumn = getCurrentColumn();
        
        if (codeType == "CHTL") {
            fragment = scanCHTLFragment();
        } else if (codeType == "CHTL_JS") {
            fragment = scanCHTLJSFragment();
        } else if (codeType == "CSS") {
            fragment = scanCSSFragment();
        } else if (codeType == "JS") {
            fragment = scanJSFragment();
        } else {
            // 默认作为CHTL处理
            fragment = scanCHTLFragment();
        }
        
        if (!fragment.content.empty()) {
            result.fragments.push_back(fragment);
        }
    }
    
    return result;
}

CodeFragment CHTLUnifiedScanner::scanCHTLFragment() {
    CodeFragment fragment;
    fragment.type = "CHTL";
    fragment.startLine = getCurrentLine();
    fragment.startColumn = getCurrentColumn();
    
    size_t startPos = position;
    
    while (position < source.length()) {
        char c = currentChar();
        
        // 检查是否为其他代码类型的开始
        if (isCHTLJSBoundary() || isCSSBoundary() || isJSBoundary()) {
            break;
        }
        
        // 处理字符串和注释
        if (c == '"' || c == '\'') {
            skipString();
        } else if (c == '/' && peekChar() == '/') {
            skipComment();
        } else if (c == '/' && peekChar() == '*') {
            skipComment();
        } else if (c == '-' && peekChar() == '-') {
            skipComment();
        } else {
            advance();
        }
    }
    
    fragment.content = source.substr(startPos, position - startPos);
    fragment.endLine = getCurrentLine();
    fragment.endColumn = getCurrentColumn();
    
    return fragment;
}

CodeFragment CHTLUnifiedScanner::scanCHTLJSFragment() {
    CodeFragment fragment;
    fragment.type = "CHTL_JS";
    fragment.startLine = getCurrentLine();
    fragment.startColumn = getCurrentColumn();
    
    size_t startPos = position;
    
    while (position < source.length()) {
        char c = currentChar();
        
        // 检查是否为其他代码类型的开始
        if (isCHTLBoundary() || isCSSBoundary() || isJSBoundary()) {
            break;
        }
        
        // 处理字符串和注释
        if (c == '"' || c == '\'') {
            skipString();
        } else if (c == '/' && peekChar() == '/') {
            skipComment();
        } else if (c == '/' && peekChar() == '*') {
            skipComment();
        } else {
            advance();
        }
    }
    
    fragment.content = source.substr(startPos, position - startPos);
    fragment.endLine = getCurrentLine();
    fragment.endColumn = getCurrentColumn();
    
    return fragment;
}

CodeFragment CHTLUnifiedScanner::scanCSSFragment() {
    CodeFragment fragment;
    fragment.type = "CSS";
    fragment.startLine = getCurrentLine();
    fragment.startColumn = getCurrentColumn();
    
    size_t startPos = position;
    
    while (position < source.length()) {
        char c = currentChar();
        
        // 检查是否为其他代码类型的开始
        if (isCHTLBoundary() || isCHTLJSBoundary() || isJSBoundary()) {
            break;
        }
        
        // 处理字符串和注释
        if (c == '"' || c == '\'') {
            skipString();
        } else if (c == '/' && peekChar() == '*') {
            skipComment();
        } else {
            advance();
        }
    }
    
    fragment.content = source.substr(startPos, position - startPos);
    fragment.endLine = getCurrentLine();
    fragment.endColumn = getCurrentColumn();
    
    return fragment;
}

CodeFragment CHTLUnifiedScanner::scanJSFragment() {
    CodeFragment fragment;
    fragment.type = "JS";
    fragment.startLine = getCurrentLine();
    fragment.startColumn = getCurrentColumn();
    
    size_t startPos = position;
    
    while (position < source.length()) {
        char c = currentChar();
        
        // 检查是否为其他代码类型的开始
        if (isCHTLBoundary() || isCHTLJSBoundary() || isCSSBoundary()) {
            break;
        }
        
        // 处理字符串和注释
        if (c == '"' || c == '\'') {
            skipString();
        } else if (c == '/' && peekChar() == '/') {
            skipComment();
        } else if (c == '/' && peekChar() == '*') {
            skipComment();
        } else {
            advance();
        }
    }
    
    fragment.content = source.substr(startPos, position - startPos);
    fragment.endLine = getCurrentLine();
    fragment.endColumn = getCurrentColumn();
    
    return fragment;
}

std::string CHTLUnifiedScanner::identifyCodeType() {
    // 检查CHTL语法特征
    if (isCHTLSyntax()) {
        return "CHTL";
    }
    
    // 检查CHTL JS语法特征
    if (isCHTLJSSyntax()) {
        return "CHTL_JS";
    }
    
    // 检查CSS语法特征
    if (isCSSSyntax()) {
        return "CSS";
    }
    
    // 检查JS语法特征
    if (isJSSyntax()) {
        return "JS";
    }
    
    // 默认作为CHTL处理
    return "CHTL";
}

bool CHTLUnifiedScanner::isCHTLSyntax() {
    // 检查CHTL特有的语法特征
    size_t savedPos = position;
    
    // 检查模板关键字
    if (position < source.length() && source[position] == '[') {
        std::string keyword;
        size_t start = position;
        advance(); // 跳过 '['
        
        while (position < source.length() && 
               source[position] != ']' && 
               !std::isspace(source[position])) {
            keyword += source[position];
            advance();
        }
        
        if (position < source.length() && source[position] == ']') {
            advance(); // 跳过 ']'
            
            if (keyword == "Template" || keyword == "Custom" || 
                keyword == "Origin" || keyword == "Import" || 
                keyword == "Namespace" || keyword == "Configuration") {
                position = savedPos;
                return true;
            }
        }
    }
    
    position = savedPos;
    
    // 检查use语句
    if (position + 3 < source.length() && 
        source.substr(position, 3) == "use") {
        position = savedPos;
        return true;
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isCHTLJSSyntax() {
    // 检查CHTL JS特有的语法特征
    size_t savedPos = position;
    
    // 检查CHTL JS函数语法
    if (position < source.length() && std::isalpha(source[position])) {
        std::string keyword;
        while (position < source.length() && 
               (std::isalnum(source[position]) || source[position] == '_')) {
            keyword += source[position];
            advance();
        }
        
        // 跳过空白
        while (position < source.length() && std::isspace(source[position])) {
            advance();
        }
        
        // 检查是否有花括号
        if (position < source.length() && source[position] == '{') {
            position = savedPos;
            return true;
        }
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isCSSSyntax() {
    // 检查CSS语法特征
    size_t savedPos = position;
    
    // 检查CSS选择器或属性
    if (position < source.length() && 
        (source[position] == '.' || source[position] == '#' || 
         std::isalpha(source[position]))) {
        
        std::string content;
        while (position < source.length() && 
               source[position] != '{' && 
               source[position] != ';' && 
               source[position] != '\n') {
            content += source[position];
            advance();
        }
        
        // 检查是否有冒号（属性）
        if (position < source.length() && source[position] == ':') {
            position = savedPos;
            return true;
        }
        
        // 检查是否有花括号（选择器）
        if (position < source.length() && source[position] == '{') {
            position = savedPos;
            return true;
        }
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isJSSyntax() {
    // 检查JS语法特征
    size_t savedPos = position;
    
    // 检查JS关键字或函数
    if (position < source.length() && 
        (std::isalpha(source[position]) || source[position] == '_' || 
         source[position] == '$' || source[position] == '(' || 
         source[position] == '[' || source[position] == '{')) {
        
        // 简单的JS语法检测
        char c = source[position];
        if (c == '(' || c == '[' || c == '{' || 
            (std::isalpha(c) && position + 1 < source.length() && 
             (source[position + 1] == '(' || source[position + 1] == '='))) {
            position = savedPos;
            return true;
        }
    }
    
    position = savedPos;
    return false;
}

char CHTLUnifiedScanner::currentChar() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char CHTLUnifiedScanner::peekChar(size_t offset) const {
    if (position + offset >= source.length()) {
        return '\0';
    }
    return source[position + offset];
}

void CHTLUnifiedScanner::advance() {
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

void CHTLUnifiedScanner::advance(size_t count) {
    for (size_t i = 0; i < count && position < source.length(); ++i) {
        advance();
    }
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (position < source.length() && std::isspace(source[position])) {
        advance();
    }
}

void CHTLUnifiedScanner::skipLine() {
    while (position < source.length() && source[position] != '\n') {
        advance();
    }
    if (position < source.length()) {
        advance(); // 跳过换行符
    }
}

void CHTLUnifiedScanner::updatePosition() {
    // 位置信息已在advance()中更新
}

void CHTLUnifiedScanner::addError(const std::string& message) {
    errors.push_back(message);
}

void CHTLUnifiedScanner::addError(const std::string& message, size_t line, size_t column) {
    errors.push_back("第" + std::to_string(line) + "行第" + std::to_string(column) + "列: " + message);
}

void CHTLUnifiedScanner::addWarning(const std::string& message) {
    warnings.push_back(message);
}

void CHTLUnifiedScanner::addWarning(const std::string& message, size_t line, size_t column) {
    warnings.push_back("第" + std::to_string(line) + "行第" + std::to_string(column) + "列: " + message);
}

bool CHTLUnifiedScanner::isCHTLBoundary() {
    return isCHTLSyntax();
}

bool CHTLUnifiedScanner::isCHTLJSBoundary() {
    return isCHTLJSSyntax();
}

bool CHTLUnifiedScanner::isCSSBoundary() {
    return isCSSSyntax();
}

bool CHTLUnifiedScanner::isJSBoundary() {
    return isJSSyntax();
}

std::string CHTLUnifiedScanner::scanString() {
    std::string str;
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    while (position < source.length() && currentChar() != quote) {
        if (currentChar() == '\\' && peekChar() == quote) {
            advance(); // 跳过反斜杠
            str += currentChar();
            advance();
        } else {
            str += currentChar();
            advance();
        }
    }
    
    if (position < source.length()) {
        advance(); // 跳过结束引号
    }
    
    return str;
}

std::string CHTLUnifiedScanner::scanComment() {
    std::string comment;
    
    if (currentChar() == '/' && peekChar() == '/') {
        // 单行注释
        comment += currentChar();
        advance();
        comment += currentChar();
        advance();
        
        while (position < source.length() && currentChar() != '\n') {
            comment += currentChar();
            advance();
        }
    } else if (currentChar() == '/' && peekChar() == '*') {
        // 多行注释
        comment += currentChar();
        advance();
        comment += currentChar();
        advance();
        
        while (position < source.length()) {
            if (currentChar() == '*' && peekChar() == '/') {
                comment += currentChar();
                advance();
                comment += currentChar();
                advance();
                break;
            }
            comment += currentChar();
            advance();
        }
    }
    
    return comment;
}

void CHTLUnifiedScanner::skipString() {
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    while (position < source.length() && currentChar() != quote) {
        if (currentChar() == '\\' && peekChar() == quote) {
            advance(); // 跳过反斜杠
            advance(); // 跳过引号
        } else {
            advance();
        }
    }
    
    if (position < source.length()) {
        advance(); // 跳过结束引号
    }
}

void CHTLUnifiedScanner::skipComment() {
    if (currentChar() == '/' && peekChar() == '/') {
        skipLine();
    } else if (currentChar() == '/' && peekChar() == '*') {
        advance(); // 跳过 '/'
        advance(); // 跳过 '*'
        
        while (position < source.length()) {
            if (currentChar() == '*' && peekChar() == '/') {
                advance(); // 跳过 '*'
                advance(); // 跳过 '/'
                break;
            }
            advance();
        }
    }
}

void CHTLUnifiedScanner::clearMessages() {
    errors.clear();
    warnings.clear();
}

void CHTLUnifiedScanner::reset() {
    position = 0;
    line = 1;
    column = 1;
    placeholderCounter = 0;
    clearMessages();
}

bool CHTLUnifiedScanner::validate(const ScanResult& result) const {
    // 验证扫描结果
    if (result.fragments.empty()) {
        return false;
    }
    
    // 检查是否有错误
    if (!result.errors.empty()) {
        return false;
    }
    
    return true;
}

std::string CHTLUnifiedScanner::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTL统一扫描器调试信息:\n";
    oss << "位置: " << position << "\n";
    oss << "行: " << line << ", 列: " << column << "\n";
    oss << "错误数: " << errors.size() << "\n";
    oss << "警告数: " << warnings.size() << "\n";
    oss << "占位符数: " << placeholderCounter << "\n";
    return oss.str();
}

} // namespace CHTL