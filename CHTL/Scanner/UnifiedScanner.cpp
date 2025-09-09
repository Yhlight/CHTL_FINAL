#include "UnifiedScanner.h"
#include <algorithm>
#include <sstream>
#include <chrono>
#include <cctype>

namespace CHTL {

UnifiedScanner::UnifiedScanner() 
    : position(0), line(1), column(1), placeholderCounter(0), debugMode(false) {
    state = {};
    stats = {};
}

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& source) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    this->source = source;
    position = 0;
    line = 1;
    column = 1;
    state = {};
    clearPlaceholders();
    
    std::vector<CodeFragment> fragments;
    
    try {
        while (position < source.length()) {
            skipWhitespace();
            
            if (position >= source.length()) {
                break;
            }
            
            CodeFragment fragment;
            
            if (isCHTLStart()) {
                fragment = scanCHTLBlock();
            } else if (isCSSStart()) {
                fragment = scanCSSBlock();
            } else if (isHTMLStart()) {
                fragment = scanHTMLBlock();
            } else {
                fragment = scanUnknownBlock();
            }
            
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
                
                // 更新统计信息
                stats.totalFragments++;
                switch (fragment.type) {
                    case CodeType::CHTL: stats.chtlFragments++; break;
                    case CodeType::CHTL_JS: stats.chtlJSFragments++; break;
                    case CodeType::CSS: stats.cssFragments++; break;
                    case CodeType::JS: stats.jsFragments++; break;
                    case CodeType::HTML: stats.htmlFragments++; break;
                    case CodeType::UNKNOWN: stats.unknownFragments++; break;
                }
            }
        }
    } catch (const std::exception& e) {
        if (debugMode) {
            std::cerr << "Scanner error: " << e.what() << std::endl;
        }
        throw;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    stats.scanTime = duration.count();
    
    return fragments;
}

std::vector<std::string> UnifiedScanner::scanAsStrings(const std::string& source) {
    std::vector<CodeFragment> fragments = scan(source);
    std::vector<std::string> result;
    
    for (const auto& fragment : fragments) {
        result.push_back(fragment.content);
    }
    
    return result;
}

char UnifiedScanner::currentChar() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char UnifiedScanner::peekChar(size_t offset) const {
    size_t pos = position + offset;
    if (pos >= source.length()) {
        return '\0';
    }
    return source[pos];
}

void UnifiedScanner::advance() {
    if (position < source.length()) {
        if (currentChar() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void UnifiedScanner::skipWhitespace() {
    while (position < source.length() && std::isspace(currentChar()) && currentChar() != '\n') {
        advance();
    }
}

void UnifiedScanner::skipNewline() {
    while (position < source.length() && currentChar() == '\n') {
        advance();
    }
}

bool UnifiedScanner::isCHTLStart() const {
    // 检查是否为CHTL语法开始
    if (currentChar() == '[') {
        // 检查是否为模板、自定义、原始嵌入等
        std::string keyword = "";
        size_t pos = position + 1;
        while (pos < source.length() && std::isalpha(source[pos])) {
            keyword += source[pos];
            pos++;
        }
        
        return keyword == "Template" || keyword == "Custom" || 
               keyword == "Origin" || keyword == "Import" ||
               keyword == "Namespace" || keyword == "Configuration" ||
               keyword == "Info" || keyword == "Export";
    }
    
    // 检查是否为HTML元素
    if (std::isalpha(currentChar())) {
        return true;
    }
    
    // 检查是否为text块
    if (position + 4 < source.length() && 
        source.substr(position, 4) == "text") {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSEnd() const {
    // CHTL JS块的结束条件
    return currentChar() == '}' && state.braceLevel == 0;
}

bool UnifiedScanner::isCSSStart() const {
    // CSS块开始检测
    return currentChar() == '{' && state.braceLevel > 0;
}

bool UnifiedScanner::isJSEnd() const {
    // JS块结束检测
    return currentChar() == '}' && state.braceLevel == 0;
}

bool UnifiedScanner::isHTMLStart() const {
    // HTML开始检测
    return currentChar() == '<';
}

bool UnifiedScanner::isHTMLEnd() const {
    // HTML结束检测
    return currentChar() == '>';
}

CodeFragment UnifiedScanner::scanCHTLBlock() {
    CodeFragment fragment;
    fragment.type = CodeType::CHTL;
    fragment.startLine = line;
    fragment.startColumn = column;
    
    size_t startPos = position;
    std::string content;
    
    while (position < source.length()) {
        char c = currentChar();
        
        if (c == '{') {
            state.braceLevel++;
            content += c;
            advance();
        } else if (c == '}') {
            state.braceLevel--;
            content += c;
            advance();
            
            if (state.braceLevel == 0) {
                break;
            }
        } else if (c == '[') {
            state.bracketLevel++;
            content += c;
            advance();
        } else if (c == ']') {
            state.bracketLevel--;
            content += c;
            advance();
        } else if (c == '(') {
            state.parenLevel++;
            content += c;
            advance();
        } else if (c == ')') {
            state.parenLevel--;
            content += c;
            advance();
        } else if (c == '"' || c == '\'') {
            // 字符串处理
            char delimiter = c;
            content += c;
            advance();
            
            while (position < source.length() && currentChar() != delimiter) {
                if (currentChar() == '\\') {
                    content += currentChar();
                    advance();
                    if (position < source.length()) {
                        content += currentChar();
                        advance();
                    }
                } else {
                    content += currentChar();
                    advance();
                }
            }
            
            if (position < source.length()) {
                content += currentChar();
                advance();
            }
        } else if (c == '/' && peekChar() == '/') {
            // 单行注释
            while (position < source.length() && currentChar() != '\n') {
                content += currentChar();
                advance();
            }
        } else if (c == '/' && peekChar() == '*') {
            // 多行注释
            content += c;
            advance();
            content += currentChar();
            advance();
            
            while (position < source.length()) {
                if (currentChar() == '*' && peekChar() == '/') {
                    content += currentChar();
                    advance();
                    content += currentChar();
                    advance();
                    break;
                }
                content += currentChar();
                advance();
            }
        } else {
            content += c;
            advance();
        }
    }
    
    fragment.content = content;
    fragment.endLine = line;
    fragment.endColumn = column;
    fragment.originalContent = content;
    
    return fragment;
}

CodeFragment UnifiedScanner::scanCHTLJSBlock() {
    CodeFragment fragment;
    fragment.type = CodeType::CHTL_JS;
    fragment.startLine = line;
    fragment.startColumn = column;
    
    size_t startPos = position;
    std::string content;
    
    while (position < source.length() && !isCHTLJSEnd()) {
        char c = currentChar();
        
        if (c == '{') {
            state.braceLevel++;
        } else if (c == '}') {
            state.braceLevel--;
        }
        
        content += c;
        advance();
    }
    
    fragment.content = content;
    fragment.endLine = line;
    fragment.endColumn = column;
    fragment.originalContent = content;
    
    return fragment;
}

CodeFragment UnifiedScanner::scanCSSBlock() {
    CodeFragment fragment;
    fragment.type = CodeType::CSS;
    fragment.startLine = line;
    fragment.startColumn = column;
    
    std::string content;
    
    while (position < source.length() && !isJSEnd()) {
        char c = currentChar();
        
        if (c == '{') {
            state.braceLevel++;
        } else if (c == '}') {
            state.braceLevel--;
        }
        
        content += c;
        advance();
    }
    
    fragment.content = content;
    fragment.endLine = line;
    fragment.endColumn = column;
    fragment.originalContent = content;
    
    return fragment;
}

CodeFragment UnifiedScanner::scanJSBlock() {
    CodeFragment fragment;
    fragment.type = CodeType::JS;
    fragment.startLine = line;
    fragment.startColumn = column;
    
    std::string content;
    
    while (position < source.length() && !isJSEnd()) {
        char c = currentChar();
        
        if (c == '{') {
            state.braceLevel++;
        } else if (c == '}') {
            state.braceLevel--;
        }
        
        content += c;
        advance();
    }
    
    fragment.content = content;
    fragment.endLine = line;
    fragment.endColumn = column;
    fragment.originalContent = content;
    
    return fragment;
}

CodeFragment UnifiedScanner::scanHTMLBlock() {
    CodeFragment fragment;
    fragment.type = CodeType::HTML;
    fragment.startLine = line;
    fragment.startColumn = column;
    
    std::string content;
    
    while (position < source.length() && !isHTMLEnd()) {
        content += currentChar();
        advance();
    }
    
    if (position < source.length()) {
        content += currentChar();
        advance();
    }
    
    fragment.content = content;
    fragment.endLine = line;
    fragment.endColumn = column;
    fragment.originalContent = content;
    
    return fragment;
}

CodeFragment UnifiedScanner::scanUnknownBlock() {
    CodeFragment fragment;
    fragment.type = CodeType::UNKNOWN;
    fragment.startLine = line;
    fragment.startColumn = column;
    
    std::string content;
    
    // 扫描到下一个已知块或文件结束
    while (position < source.length()) {
        if (isCHTLStart() || isCSSStart() || isHTMLStart()) {
            break;
        }
        
        content += currentChar();
        advance();
    }
    
    fragment.content = content;
    fragment.endLine = line;
    fragment.endColumn = column;
    fragment.originalContent = content;
    
    return fragment;
}

std::string UnifiedScanner::createPlaceholder(const std::string& content) {
    std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholderCounter++);
    registerPlaceholder(placeholder, content);
    return placeholder;
}

std::string UnifiedScanner::restorePlaceholders(const std::string& content) {
    std::string result = content;
    
    for (const auto& pair : placeholders) {
        size_t pos = 0;
        while ((pos = result.find(pair.first, pos)) != std::string::npos) {
            result.replace(pos, pair.first.length(), pair.second);
            pos += pair.second.length();
        }
    }
    
    return result;
}

void UnifiedScanner::registerPlaceholder(const std::string& placeholder, const std::string& content) {
    placeholders[placeholder] = content;
}

bool UnifiedScanner::isAtEnd() const {
    return position >= source.length();
}

size_t UnifiedScanner::getCurrentPosition() const {
    return position;
}

size_t UnifiedScanner::getCurrentLine() const {
    return line;
}

size_t UnifiedScanner::getCurrentColumn() const {
    return column;
}

void UnifiedScanner::clearPlaceholders() {
    placeholders.clear();
    placeholderCounter = 0;
}

std::map<std::string, std::string> UnifiedScanner::getPlaceholders() const {
    return placeholders;
}

void UnifiedScanner::enableDebugMode(bool enable) {
    debugMode = enable;
}

std::string UnifiedScanner::getDebugInfo() const {
    std::ostringstream oss;
    oss << "UnifiedScanner Debug Info:\n";
    oss << "Position: " << position << "\n";
    oss << "Line: " << line << ", Column: " << column << "\n";
    oss << "Brace Level: " << state.braceLevel << "\n";
    oss << "Bracket Level: " << state.bracketLevel << "\n";
    oss << "Paren Level: " << state.parenLevel << "\n";
    return oss.str();
}

UnifiedScanner::ScanStats UnifiedScanner::getStats() const {
    return stats;
}

void UnifiedScanner::resetStats() {
    stats = {};
}

void UnifiedScanner::throwError(const std::string& message) const {
    std::ostringstream oss;
    oss << "Scanner error at line " << line << ", column " << column << ": " << message;
    throw std::runtime_error(oss.str());
}

void UnifiedScanner::synchronize() {
    // 同步到下一个安全位置
    while (position < source.length()) {
        if (currentChar() == '\n') {
            advance();
            break;
        }
        advance();
    }
}

} // namespace CHTL