#include "UnifiedScanner.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL {

UnifiedScanner::UnifiedScanner()
    : placeholderPrefix_("_PLACEHOLDER_"), enablePlaceholderMechanism_(true), 
      currentPos_(0), currentLine_(1), currentColumn_(1), placeholderCounter_(0) {
}

ScanResult UnifiedScanner::scan(const std::string& source) {
    ScanResult result;
    result.clear();
    
    source_ = source;
    currentPos_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
    placeholders_.clear();
    placeholderCounter_ = 0;
    
    std::string processedCode = source;
    
    // 使用双指针扫描识别不同类型的代码片段
    while (currentPos_ < source_.length()) {
        skipWhitespace();
        
        if (currentPos_ >= source_.length()) break;
        
        // 检测代码类型
        CodeFragmentType fragmentType = CodeFragmentType::UNKNOWN;
        
        if (isCHTLBoundary()) {
            fragmentType = CodeFragmentType::CHTL;
        } else if (isCHTLJSBoundary()) {
            fragmentType = CodeFragmentType::CHTL_JS;
        } else if (isCSSBoundary()) {
            fragmentType = CodeFragmentType::CSS;
        } else if (isJSBoundary()) {
            fragmentType = CodeFragmentType::JS;
        } else if (isHTMLBoundary()) {
            fragmentType = CodeFragmentType::HTML;
        }
        
        if (fragmentType != CodeFragmentType::UNKNOWN) {
            CodeFragment fragment = scanFragmentByType(fragmentType);
            if (!fragment.content.empty()) {
                result.fragments.push_back(fragment);
                
                // 如果启用占位符机制，替换代码
                if (enablePlaceholderMechanism_) {
                    std::string placeholder = createPlaceholder(fragment.content);
                    processedCode.replace(fragment.startLine, fragment.content.length(), placeholder);
                    registerPlaceholder(placeholder, fragment.content);
                }
            }
        } else {
            // 未知类型，跳过当前字符
            advance();
        }
    }
    
    result.placeholders = placeholders_;
    result.processedCode = processedCode;
    
    return result;
}

ScanResult UnifiedScanner::scanStyleBlock(const std::string& styleContent) {
    ScanResult result;
    result.clear();
    
    // 专门处理样式块中的CHTL语法
    std::string processed = styleContent;
    size_t pos = 0;
    
    while (pos < styleContent.length()) {
        // 查找CHTL语法边界
        size_t start = styleContent.find("@", pos);
        if (start == std::string::npos) break;
        
        // 检查是否是CHTL语法
        if (isCHTLStyleSyntax(styleContent, start)) {
            size_t end = findStyleBlockEnd(styleContent, start);
            if (end != std::string::npos) {
                std::string fragment = styleContent.substr(start, end - start);
                
                CodeFragment codeFragment(CodeFragmentType::CHTL, fragment, 1, start, 1, end);
                result.fragments.push_back(codeFragment);
                
                // 创建占位符
                std::string placeholder = createPlaceholder(fragment);
                processed.replace(start, end - start, placeholder);
                registerPlaceholder(placeholder, fragment);
                
                pos = end;
            } else {
                pos = start + 1;
            }
        } else {
            pos = start + 1;
        }
    }
    
    result.processedCode = processed;
    result.placeholders = placeholders_;
    
    return result;
}

ScanResult UnifiedScanner::scanScriptBlock(const std::string& scriptContent) {
    ScanResult result;
    result.clear();
    
    // 专门处理脚本块中的CHTL JS和JS混合代码
    std::string processed = scriptContent;
    size_t pos = 0;
    
    while (pos < scriptContent.length()) {
        // 查找CHTL JS语法边界
        if (isCHTLJSSyntax(scriptContent, pos)) {
            size_t end = findScriptBlockEnd(scriptContent, pos);
            if (end != std::string::npos) {
                std::string fragment = scriptContent.substr(pos, end - pos);
                
                CodeFragment codeFragment(CodeFragmentType::CHTL_JS, fragment, 1, pos, 1, end);
                result.fragments.push_back(codeFragment);
                
                // 创建占位符
                std::string placeholder = createPlaceholder(fragment);
                processed.replace(pos, end - pos, placeholder);
                registerPlaceholder(placeholder, fragment);
                
                pos = end;
            } else {
                pos++;
            }
        } else {
            pos++;
        }
    }
    
    result.processedCode = processed;
    result.placeholders = placeholders_;
    
    return result;
}

char UnifiedScanner::currentChar() const {
    return (currentPos_ < source_.length()) ? source_[currentPos_] : '\0';
}

char UnifiedScanner::peekChar(size_t offset) const {
    size_t pos = currentPos_ + offset;
    return (pos < source_.length()) ? source_[pos] : '\0';
}

void UnifiedScanner::advance() {
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

void UnifiedScanner::skipWhitespace() {
    while (currentPos_ < source_.length() && isWhitespace(currentChar())) {
        advance();
    }
}

bool UnifiedScanner::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool UnifiedScanner::isCHTLBoundary() const {
    // 检测CHTL语法边界
    if (currentChar() == '[') {
        // 检查是否是CHTL块关键字
        std::string keyword = extractKeyword();
        return (keyword == "Template" || keyword == "Custom" || keyword == "Origin" ||
                keyword == "Import" || keyword == "Configuration" || keyword == "Namespace");
    }
    
    // 检查是否是HTML元素
    if (std::isalpha(currentChar()) || currentChar() == '_') {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSBoundary() const {
    // 检测CHTL JS语法边界
    std::string keyword = extractKeyword();
    return (keyword == "Vir" || keyword == "Listen" || keyword == "Animate" ||
            keyword == "Router" || keyword == "ScriptLoader" || keyword == "util");
}

bool UnifiedScanner::isCSSBoundary() const {
    // 检测CSS语法边界
    return currentChar() == '.' || currentChar() == '#' || currentChar() == '@';
}

bool UnifiedScanner::isJSBoundary() const {
    // 检测JavaScript语法边界
    return (currentChar() == 'f' && peekChar() == 'u' && peekChar(2) == 'n' && peekChar(3) == 'c') ||
           (currentChar() == 'c' && peekChar() == 'o' && peekChar(2) == 'n' && peekChar(3) == 's') ||
           (currentChar() == 'l' && peekChar() == 'e' && peekChar(2) == 't') ||
           (currentChar() == 'v' && peekChar() == 'a' && peekChar(2) == 'r') ||
           (currentChar() == 'i' && peekChar() == 'f') ||
           (currentChar() == 'f' && peekChar() == 'o' && peekChar(2) == 'r') ||
           (currentChar() == 'w' && peekChar() == 'h' && peekChar(2) == 'i' && peekChar(3) == 'l') ||
           (currentChar() == 'r' && peekChar() == 'e' && peekChar(2) == 't' && peekChar(3) == 'u');
}

bool UnifiedScanner::isHTMLBoundary() const {
    // 检测HTML语法边界
    return currentChar() == '<';
}

bool UnifiedScanner::isStyleBlock() const {
    return extractKeyword() == "style";
}

bool UnifiedScanner::isScriptBlock() const {
    return extractKeyword() == "script";
}

bool UnifiedScanner::isTemplateBlock() const {
    return extractKeyword() == "Template";
}

bool UnifiedScanner::isCustomBlock() const {
    return extractKeyword() == "Custom";
}

bool UnifiedScanner::isOriginBlock() const {
    return extractKeyword() == "Origin";
}

bool UnifiedScanner::isImportBlock() const {
    return extractKeyword() == "Import";
}

bool UnifiedScanner::isConfigurationBlock() const {
    return extractKeyword() == "Configuration";
}

bool UnifiedScanner::isNamespaceBlock() const {
    return extractKeyword() == "Namespace";
}

CodeFragment UnifiedScanner::scanFragmentByType(CodeFragmentType type) {
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    size_t startPos = currentPos_;
    
    std::string content;
    
    switch (type) {
        case CodeFragmentType::CHTL:
            content = scanCHTLContent();
            break;
        case CodeFragmentType::CHTL_JS:
            content = scanCHTLJSContent();
            break;
        case CodeFragmentType::CSS:
            content = scanCSSContent();
            break;
        case CodeFragmentType::JS:
            content = scanJSContent();
            break;
        case CodeFragmentType::HTML:
            content = scanHTMLContent();
            break;
        default:
            content = "";
            break;
    }
    
    size_t endLine = currentLine_;
    size_t endColumn = currentColumn_;
    
    return CodeFragment(type, content, startLine, startColumn, endLine, endColumn);
}

std::string UnifiedScanner::scanCHTLContent() {
    std::string content;
    
    if (currentChar() == '[') {
        // 扫描CHTL块
        content += currentChar();
        advance();
        
        // 扫描块名称
        while (currentPos_ < source_.length() && currentChar() != ']') {
            content += currentChar();
            advance();
        }
        
        if (currentChar() == ']') {
            content += currentChar();
            advance();
        }
        
        // 扫描块内容
        if (currentChar() == '{') {
            content += currentChar();
            advance();
            
            int braceCount = 1;
            while (currentPos_ < source_.length() && braceCount > 0) {
                if (currentChar() == '{') braceCount++;
                else if (currentChar() == '}') braceCount--;
                
                content += currentChar();
                advance();
            }
        }
    } else {
        // 扫描HTML元素
        while (currentPos_ < source_.length() && 
               !isWhitespace(currentChar()) && 
               currentChar() != '{' && 
               currentChar() != '}') {
            content += currentChar();
            advance();
        }
        
        // 扫描属性
        while (currentPos_ < source_.length() && currentChar() != '{') {
            content += currentChar();
            advance();
        }
        
        // 扫描块内容
        if (currentChar() == '{') {
            content += currentChar();
            advance();
            
            int braceCount = 1;
            while (currentPos_ < source_.length() && braceCount > 0) {
                if (currentChar() == '{') braceCount++;
                else if (currentChar() == '}') braceCount--;
                
                content += currentChar();
                advance();
            }
        }
    }
    
    return content;
}

std::string UnifiedScanner::scanCHTLJSContent() {
    std::string content;
    
    // 扫描CHTL JS关键字
    while (currentPos_ < source_.length() && 
           !isWhitespace(currentChar()) && 
           currentChar() != '{' && 
           currentChar() != '}') {
        content += currentChar();
        advance();
    }
    
    // 扫描块内容
    if (currentChar() == '{') {
        content += currentChar();
        advance();
        
        int braceCount = 1;
        while (currentPos_ < source_.length() && braceCount > 0) {
            if (currentChar() == '{') braceCount++;
            else if (currentChar() == '}') braceCount--;
            
            content += currentChar();
            advance();
        }
    }
    
    return content;
}

std::string UnifiedScanner::scanCSSContent() {
    std::string content;
    
    // 扫描CSS选择器
    while (currentPos_ < source_.length() && currentChar() != '{') {
        content += currentChar();
        advance();
    }
    
    // 扫描CSS规则
    if (currentChar() == '{') {
        content += currentChar();
        advance();
        
        int braceCount = 1;
        while (currentPos_ < source_.length() && braceCount > 0) {
            if (currentChar() == '{') braceCount++;
            else if (currentChar() == '}') braceCount--;
            
            content += currentChar();
            advance();
        }
    }
    
    return content;
}

std::string UnifiedScanner::scanJSContent() {
    std::string content;
    
    // 扫描JavaScript代码
    while (currentPos_ < source_.length()) {
        if (isCHTLJSBoundary()) {
            break;
        }
        
        content += currentChar();
        advance();
    }
    
    return content;
}

std::string UnifiedScanner::scanHTMLContent() {
    std::string content;
    
    // 扫描HTML标签
    if (currentChar() == '<') {
        content += currentChar();
        advance();
        
        while (currentPos_ < source_.length() && currentChar() != '>') {
            content += currentChar();
            advance();
        }
        
        if (currentChar() == '>') {
            content += currentChar();
            advance();
        }
    }
    
    return content;
}

std::string UnifiedScanner::createPlaceholder(const std::string& content) {
    std::ostringstream oss;
    oss << placeholderPrefix_ << placeholderCounter_++ << "_";
    return oss.str();
}

std::string UnifiedScanner::restorePlaceholders(const std::string& processedCode) {
    std::string result = processedCode;
    
    for (const auto& placeholder : placeholders_) {
        size_t pos = 0;
        while ((pos = result.find(placeholder.first, pos)) != std::string::npos) {
            result.replace(pos, placeholder.first.length(), placeholder.second);
            pos += placeholder.second.length();
        }
    }
    
    return result;
}

void UnifiedScanner::registerPlaceholder(const std::string& placeholder, const std::string& content) {
    placeholders_[placeholder] = content;
}

void UnifiedScanner::setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler) {
    errorHandler_ = handler;
}

void UnifiedScanner::reportError(const std::string& message) {
    if (errorHandler_) {
        errorHandler_(message, currentLine_, currentColumn_);
    } else {
        std::cerr << "Scanner Error at " << currentLine_ << ":" << currentColumn_ << ": " << message << std::endl;
    }
}

std::string UnifiedScanner::extractKeyword() const {
    std::string keyword;
    size_t pos = currentPos_;
    
    // 跳过开头的非字母字符
    while (pos < source_.length() && !std::isalpha(source_[pos])) {
        pos++;
    }
    
    // 提取关键字
    while (pos < source_.length() && (std::isalnum(source_[pos]) || source_[pos] == '_')) {
        keyword += source_[pos];
        pos++;
    }
    
    return keyword;
}

bool UnifiedScanner::isCHTLStyleSyntax(const std::string& content, size_t pos) const {
    // 检查是否是CHTL样式语法
    if (pos + 1 < content.length() && content[pos] == '@') {
        char next = content[pos + 1];
        return std::isalpha(next) || next == '_';
    }
    return false;
}

bool UnifiedScanner::isCHTLJSSyntax(const std::string& content, size_t pos) const {
    // 检查是否是CHTL JS语法
    std::string keyword = extractKeywordFromPos(content, pos);
    return (keyword == "Vir" || keyword == "Listen" || keyword == "Animate" ||
            keyword == "Router" || keyword == "ScriptLoader" || keyword == "util");
}

std::string UnifiedScanner::extractKeywordFromPos(const std::string& content, size_t pos) const {
    std::string keyword;
    
    // 跳过开头的非字母字符
    while (pos < content.length() && !std::isalpha(content[pos])) {
        pos++;
    }
    
    // 提取关键字
    while (pos < content.length() && (std::isalnum(content[pos]) || content[pos] == '_')) {
        keyword += content[pos];
        pos++;
    }
    
    return keyword;
}

size_t UnifiedScanner::findStyleBlockEnd(const std::string& content, size_t start) const {
    size_t pos = start;
    int braceCount = 0;
    bool inString = false;
    char stringChar = 0;
    
    while (pos < content.length()) {
        char c = content[pos];
        
        if (!inString) {
            if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            } else if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
                if (braceCount == 0) {
                    return pos + 1;
                }
            }
        } else {
            if (c == stringChar && (pos == 0 || content[pos - 1] != '\\')) {
                inString = false;
            }
        }
        
        pos++;
    }
    
    return std::string::npos;
}

size_t UnifiedScanner::findScriptBlockEnd(const std::string& content, size_t start) const {
    size_t pos = start;
    int braceCount = 0;
    bool inString = false;
    char stringChar = 0;
    
    while (pos < content.length()) {
        char c = content[pos];
        
        if (!inString) {
            if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            } else if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
                if (braceCount == 0) {
                    return pos + 1;
                }
            }
        } else {
            if (c == stringChar && (pos == 0 || content[pos - 1] != '\\')) {
                inString = false;
            }
        }
        
        pos++;
    }
    
    return std::string::npos;
}

} // namespace CHTL