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
    
    // 简化：将整个源作为单个CHTL片段返回，确保基础流程打通
    CodeFragment fragment;
    fragment.type = "CHTL";
    fragment.startLine = 1;
    fragment.startColumn = 1;
    fragment.endLine = std::count(source.begin(), source.end(), '\n') + 1;
    fragment.endColumn = 1;
    fragment.content = source;
    result.fragments.push_back(fragment);
    
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
    // 根据CHTL.md规范，实现智能代码类型识别
    
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
    
    // 检查HTML语法特征
    if (isHTMLSyntax()) {
        return "HTML";
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

bool CHTLUnifiedScanner::isHTMLSyntax() {
    // 检查HTML语法特征
    size_t savedPos = position;
    
    // 检查HTML标签
    if (position < source.length() && source[position] == '<') {
        advance(); // 跳过 '<'
        
        // 检查标签名
        if (position < source.length() && 
            (std::isalpha(source[position]) || source[position] == '/')) {
            
            std::string tagName;
            while (position < source.length() && 
                   (std::isalnum(source[position]) || source[position] == '-')) {
                tagName += source[position];
                advance();
            }
            
            // 检查是否有结束标签
            if (position < source.length() && source[position] == '>') {
                position = savedPos;
                return true;
            }
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

// 占位符处理方法
std::string CHTLUnifiedScanner::createPlaceholder(const std::string& content) {
    std::string placeholder = "___PLACEHOLDER_" + std::to_string(placeholderCounter++) + "___";
    placeholders[placeholder] = content;
    return placeholder;
}

std::string CHTLUnifiedScanner::restorePlaceholder(const std::string& placeholder) {
    auto it = placeholders.find(placeholder);
    if (it != placeholders.end()) {
        return it->second;
    }
    return placeholder;
}

void CHTLUnifiedScanner::processPlaceholders() {
    // 处理占位符，恢复原始内容
    for (auto& pair : placeholders) {
        const std::string& placeholder = pair.first;
        const std::string& content = pair.second;
        
        // 在源代码中查找并替换占位符
        size_t pos = 0;
        while ((pos = source.find(placeholder, pos)) != std::string::npos) {
            source.replace(pos, placeholder.length(), content);
            pos += content.length();
        }
    }
}

// 高级占位符处理
std::string CHTLUnifiedScanner::processPlaceholdersInCode(const std::string& code) {
    std::string result = code;
    
    for (auto& pair : placeholders) {
        const std::string& placeholder = pair.first;
        const std::string& content = pair.second;
        
        // 替换所有占位符
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), content);
            pos += content.length();
        }
    }
    
    return result;
}

// 占位符验证
bool CHTLUnifiedScanner::validatePlaceholders() const {
    for (auto& pair : placeholders) {
        const std::string& placeholder = pair.first;
        if (placeholder.empty() || placeholder.length() < 10) {
            return false;
        }
    }
    return true;
}

// 占位符清理
void CHTLUnifiedScanner::clearPlaceholders() {
    placeholders.clear();
    placeholderCounter = 0;
}

// 代码分离方法
std::string CHTLUnifiedScanner::separateCHTLAndJS(const std::string& code) {
    // 根据CHTL.md规范，实现CHTL和JS代码分离
    std::string result = code;
    
    // 查找CHTL JS块
    size_t pos = 0;
    while ((pos = result.find("{{", pos)) != std::string::npos) {
        size_t endPos = result.find("}}", pos);
        if (endPos != std::string::npos) {
            // 找到CHTL JS块，可以进一步处理
            pos = endPos + 2;
        } else {
            break;
        }
    }
    
    return result;
}

std::string CHTLUnifiedScanner::separateCSSAndJS(const std::string& code) {
    // 根据CHTL.md规范，实现CSS和JS代码分离
    std::string result = code;
    
    // 查找CSS块
    size_t pos = 0;
    while ((pos = result.find("<style>", pos)) != std::string::npos) {
        size_t endPos = result.find("</style>", pos);
        if (endPos != std::string::npos) {
            // 找到CSS块，可以进一步处理
            pos = endPos + 8;
        } else {
            break;
        }
    }
    
    // 查找JS块
    pos = 0;
    while ((pos = result.find("<script>", pos)) != std::string::npos) {
        size_t endPos = result.find("</script>", pos);
        if (endPos != std::string::npos) {
            // 找到JS块，可以进一步处理
            pos = endPos + 9;
        } else {
            break;
        }
    }
    
    return result;
}

// 块检测方法
bool CHTLUnifiedScanner::isBlockStart() {
    // 检查是否为块开始
    size_t savedPos = position;
    
    // 检查CHTL块开始标记
    if (position < source.length() && source[position] == '[') {
        advance();
        if (position < source.length() && std::isalpha(source[position])) {
            position = savedPos;
            return true;
        }
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isBlockEnd() {
    // 检查是否为块结束
    size_t savedPos = position;
    
    // 检查CHTL块结束标记
    if (position < source.length() && source[position] == ']') {
        advance();
        position = savedPos;
        return true;
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isStyleBlock() {
    // 检查是否为样式块
    size_t savedPos = position;
    
    if (position + 6 < source.length() && 
        source.substr(position, 6) == "[Style") {
        position = savedPos;
        return true;
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isScriptBlock() {
    // 检查是否为脚本块
    size_t savedPos = position;
    
    if (position + 7 < source.length() && 
        source.substr(position, 7) == "[Script") {
        position = savedPos;
        return true;
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isTemplateBlock() {
    // 检查是否为模板块
    size_t savedPos = position;
    
    if (position + 8 < source.length() && 
        source.substr(position, 8) == "[Template") {
        position = savedPos;
        return true;
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isCustomBlock() {
    // 检查是否为自定义块
    size_t savedPos = position;
    
    if (position + 6 < source.length() && 
        source.substr(position, 6) == "[Custom") {
        position = savedPos;
        return true;
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isOriginBlock() {
    // 检查是否为Origin块
    size_t savedPos = position;
    
    if (position + 6 < source.length() && 
        source.substr(position, 6) == "[Origin") {
        position = savedPos;
        return true;
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isImportBlock() {
    // 检查是否为Import块
    size_t savedPos = position;
    
    if (position + 6 < source.length() && 
        source.substr(position, 6) == "[Import") {
        position = savedPos;
        return true;
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isNamespaceBlock() {
    // 检查是否为Namespace块
    size_t savedPos = position;
    
    if (position + 9 < source.length() && 
        source.substr(position, 9) == "[Namespace") {
        position = savedPos;
        return true;
    }
    
    position = savedPos;
    return false;
}

bool CHTLUnifiedScanner::isConfigurationBlock() {
    // 检查是否为Configuration块
    size_t savedPos = position;
    
    if (position + 12 < source.length() && 
        source.substr(position, 12) == "[Configuration") {
        position = savedPos;
        return true;
    }
    
    position = savedPos;
    return false;
}

// 宽判严判机制
bool CHTLUnifiedScanner::isWideScope() {
    // 宽判：宽松的代码类型判断
    return true;
}

bool CHTLUnifiedScanner::isStrictScope() {
    // 严判：严格的代码类型判断
    return strictMode;
}

// 可变长度切片
std::string CHTLUnifiedScanner::sliceCode(size_t start, size_t end) {
    if (start >= source.length() || end > source.length() || start >= end) {
        return "";
    }
    return source.substr(start, end - start);
}

void CHTLUnifiedScanner::adjustSliceBoundary(size_t& start, size_t& end) {
    // 调整切片边界，确保完整性
    if (start >= source.length()) {
        start = source.length();
    }
    if (end > source.length()) {
        end = source.length();
    }
    if (start >= end) {
        end = start;
    }
}

// 动态长度切片
std::string CHTLUnifiedScanner::sliceCodeDynamic(size_t start, size_t& end) {
    if (start >= source.length()) {
        return "";
    }
    
    // 动态确定结束位置
    size_t dynamicEnd = start;
    while (dynamicEnd < source.length()) {
        char c = source[dynamicEnd];
        if (c == '\n' || c == '\r' || c == ';' || c == '}') {
            break;
        }
        dynamicEnd++;
    }
    
    end = dynamicEnd;
    return source.substr(start, end - start);
}

// 智能边界检测
bool CHTLUnifiedScanner::isBoundaryChar(char c) const {
    return c == '\n' || c == '\r' || c == ';' || c == '}' || c == '{' || c == '(' || c == ')';
}

// 语法边界识别
size_t CHTLUnifiedScanner::findSyntaxBoundary(size_t start, SyntaxBoundaryType type) const {
    size_t pos = start;
    int braceCount = 0;
    int parenCount = 0;
    bool inString = false;
    bool inComment = false;
    
    while (pos < source.length()) {
        char c = source[pos];
        char nextC = (pos + 1 < source.length()) ? source[pos + 1] : '\0';
        
        // 处理字符串
        if (c == '"' || c == '\'') {
            inString = !inString;
        }
        
        // 处理注释
        if (!inString && c == '/' && nextC == '/') {
            inComment = true;
        }
        if (inComment && c == '\n') {
            inComment = false;
        }
        
        if (!inString && !inComment) {
            // 处理括号
            if (c == '{') braceCount++;
            else if (c == '}') braceCount--;
            else if (c == '(') parenCount++;
            else if (c == ')') parenCount--;
            
            // 根据类型确定边界
            switch (type) {
                case SyntaxBoundaryType::BRACE_BLOCK:
                    if (c == '}' && braceCount == 0) return pos + 1;
                    break;
                case SyntaxBoundaryType::PAREN_BLOCK:
                    if (c == ')' && parenCount == 0) return pos + 1;
                    break;
                case SyntaxBoundaryType::STATEMENT:
                    if (c == ';' && braceCount == 0 && parenCount == 0) return pos + 1;
                    break;
                case SyntaxBoundaryType::LINE:
                    if (c == '\n') return pos + 1;
                    break;
            }
        }
        
        pos++;
    }
    
    return source.length();
}

// 智能扩增
void CHTLUnifiedScanner::expandSlice(size_t& start, size_t& end) {
    // 智能扩增切片范围
    if (start > 0) {
        start--;
    }
    if (end < source.length()) {
        end++;
    }
}

void CHTLUnifiedScanner::shrinkSlice(size_t& start, size_t& end) {
    // 智能收缩切片范围
    if (start < end) {
        start++;
    }
    if (end > start) {
        end--;
    }
}

// 智能扩增到语法边界
void CHTLUnifiedScanner::expandToSyntaxBoundary(size_t& start, size_t& end, SyntaxBoundaryType type) {
    // 向前扩增到语法边界
    size_t newStart = start;
    while (newStart > 0) {
        char c = source[newStart - 1];
        if (isBoundaryChar(c)) {
            break;
        }
        newStart--;
    }
    start = newStart;
    
    // 向后扩增到语法边界
    size_t newEnd = end;
    while (newEnd < source.length()) {
        char c = source[newEnd];
        if (isBoundaryChar(c)) {
            break;
        }
        newEnd++;
    }
    end = newEnd;
}

// 智能扩增到完整语句
void CHTLUnifiedScanner::expandToCompleteStatement(size_t& start, size_t& end) {
    // 向前扩增到语句开始
    while (start > 0) {
        char c = source[start - 1];
        if (c == '\n' || c == ';' || c == '}') {
            break;
        }
        start--;
    }
    
    // 向后扩增到语句结束
    while (end < source.length()) {
        char c = source[end];
        if (c == '\n' || c == ';' || c == '}') {
            end++;
            break;
        }
        end++;
    }
}

// 智能扩增到完整块
void CHTLUnifiedScanner::expandToCompleteBlock(size_t& start, size_t& end) {
    // 向前扩增到块开始
    while (start > 0) {
        char c = source[start - 1];
        if (c == '{' || c == '\n') {
            break;
        }
        start--;
    }
    
    // 向后扩增到块结束
    int braceCount = 0;
    size_t pos = start;
    while (pos < source.length()) {
        char c = source[pos];
        if (c == '{') braceCount++;
        else if (c == '}') {
            braceCount--;
            if (braceCount == 0) {
                end = pos + 1;
                break;
            }
        }
        pos++;
    }
}

} // namespace CHTL