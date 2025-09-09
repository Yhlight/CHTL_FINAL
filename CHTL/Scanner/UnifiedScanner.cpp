#include "UnifiedScanner.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <sstream>

namespace CHTL {

CodeFragment::CodeFragment(CodeType type, const std::string& content, 
                          size_t startLine, size_t startColumn, 
                          size_t endLine, size_t endColumn)
    : type(type), content(content), startLine(startLine), startColumn(startColumn),
      endLine(endLine), endColumn(endColumn) {
    placeholder = createPlaceholder(type, 0);
}

UnifiedScanner::UnifiedScanner() 
    : position(0), line(1), column(1), placeholderCounter(0), debugMode(false) {
    resetStats();
}

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& source) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    this->source = source;
    position = 0;
    line = 1;
    column = 1;
    fragments.clear();
    placeholders.clear();
    placeholderCounter = 0;
    
    // 预分配内存以提高性能
    fragments.reserve(source.length() / 100); // 估算片段数量
    
    // 重置状态
    inCHTLBlock = false;
    inCHTLJSBlock = false;
    inCSSBlock = false;
    inJSBlock = false;
    inHTMLBlock = false;
    
    // 重置扫描状态
    state = ScanState{};
    
    // 使用双指针扫描优化
    size_t startPos = 0;
    while (position < source.length()) {
        skipWhitespace();
        
        if (position >= source.length()) {
            break;
        }
        
        startPos = position;
        CodeType type = identifyCodeType(position);
        
        // 使用智能边界检测
        if (!isSafeBoundary()) {
            expandBoundary();
        }
        
        CodeFragment fragment = CodeFragment{CodeType::UNKNOWN, "", 0, 0, 0, 0};
        
        switch (type) {
            case CodeType::CHTL:
                fragment = scanCHTLBlock();
                break;
            case CodeType::CHTL_JS:
                fragment = scanCHTLJSBlock();
                break;
            case CodeType::CSS:
                fragment = scanCSSBlock();
                break;
            case CodeType::JS:
                fragment = scanJSBlock();
                break;
            case CodeType::HTML:
                fragment = scanHTMLBlock();
                break;
            default:
                fragment = scanUnknownBlock();
                break;
        }
        
        if (!fragment.content.empty()) {
            // 优化占位符创建
            fragment.placeholder = createPlaceholder(fragment.type, placeholderCounter++);
            registerPlaceholder(fragment.placeholder, fragment.content);
            
            fragments.push_back(std::move(fragment));
            stats.totalFragments++;
            
            switch (fragments.back().type) {
                case CodeType::CHTL: stats.chtlFragments++; break;
                case CodeType::CHTL_JS: stats.chtlJSFragments++; break;
                case CodeType::CSS: stats.cssFragments++; break;
                case CodeType::JS: stats.jsFragments++; break;
                case CodeType::HTML: stats.htmlFragments++; break;
                case CodeType::UNKNOWN: stats.unknownFragments++; break;
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    stats.scanTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    return fragments;
}

std::vector<std::string> UnifiedScanner::scanAsStrings(const std::string& source) {
    auto fragments = scan(source);
    std::vector<std::string> result;
    
    for (const auto& fragment : fragments) {
        result.push_back(fragment.content);
    }
    
    return result;
}

CodeType UnifiedScanner::identifyCodeType(size_t startPos) const {
    // 保存当前位置
    size_t savedPos = position;
    size_t savedLine = line;
    size_t savedColumn = column;
    
    // 临时设置位置
    position = startPos;
    
    CodeType type = CodeType::UNKNOWN;
    
    if (isCHTLStart()) {
        type = CodeType::CHTL;
    } else if (isCSSStart()) {
        type = CodeType::CSS;
    } else if (isHTMLStart()) {
        type = CodeType::HTML;
    } else {
        // 检查是否为CHTL JS或普通JS
        if (isCHTLJSBlockStart()) {
            type = CodeType::CHTL_JS;
        } else {
            type = CodeType::JS;
        }
    }
    
    // 恢复位置
    position = savedPos;
    line = savedLine;
    column = savedColumn;
    
    return type;
}

bool UnifiedScanner::isCHTLStart() const {
    // 检查CHTL语法开始标记
    if (isTemplateBlock() || isCustomBlock() || isOriginBlock() || 
        isImportBlock() || isNamespaceBlock() || isConfigurationBlock()) {
        return true;
    }
    
    // 检查HTML元素
    if (isHTMLStart()) {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSBlockStart() const {
    // 检查CHTL JS关键字
    std::string keyword = "";
    size_t pos = position;
    
    while (pos < source.length() && std::isalpha(source[pos])) {
        keyword += source[pos];
        pos++;
    }
    
    return keyword == "fileloader" || keyword == "listen" || keyword == "delegate" ||
           keyword == "animate" || keyword == "vir" || keyword == "router" || keyword == "util";
}

bool UnifiedScanner::isCSSStart() const {
    // 检查CSS语法
    if (position < source.length() && source[position] == '.') {
        return true;
    }
    
    if (position + 1 < source.length() && source[position] == '#' && source[position + 1] != '{') {
        return true;
    }
    
    // 检查CSS属性
    std::string prop = "";
    size_t pos = position;
    while (pos < source.length() && (std::isalnum(source[pos]) || source[pos] == '-')) {
        prop += source[pos];
        pos++;
    }
    
    if (pos < source.length() && source[pos] == ':') {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isHTMLStart() const {
    // 检查HTML标签
    if (position < source.length() && source[position] == '<') {
        return true;
    }
    
    // 检查CHTL HTML元素
    std::string element = "";
    size_t pos = position;
    
    while (pos < source.length() && std::isalpha(source[pos])) {
        element += source[pos];
        pos++;
    }
    
    if (pos < source.length() && source[pos] == '{') {
        return element == "html" || element == "head" || element == "body" ||
               element == "div" || element == "span" || element == "p" ||
               element == "h1" || element == "h2" || element == "h3" ||
               element == "h4" || element == "h5" || element == "h6" ||
               element == "a" || element == "img" || element == "button" ||
               element == "input" || element == "form" || element == "table" ||
               element == "ul" || element == "ol" || element == "li";
    }
    
    return false;
}

bool UnifiedScanner::isTemplateBlock() const {
    return source.substr(position, 10) == "[Template]";
}

bool UnifiedScanner::isCustomBlock() const {
    return source.substr(position, 8) == "[Custom]";
}

bool UnifiedScanner::isOriginBlock() const {
    return source.substr(position, 8) == "[Origin]";
}

bool UnifiedScanner::isImportBlock() const {
    return source.substr(position, 8) == "[Import]";
}

bool UnifiedScanner::isNamespaceBlock() const {
    return source.substr(position, 11) == "[Namespace]";
}

bool UnifiedScanner::isConfigurationBlock() const {
    return source.substr(position, 14) == "[Configuration]";
}

bool UnifiedScanner::isStyleBlock() const {
    std::string keyword = "";
    size_t pos = position;
    
    while (pos < source.length() && std::isalpha(source[pos])) {
        keyword += source[pos];
        pos++;
    }
    
    return keyword == "style";
}

bool UnifiedScanner::isScriptBlock() const {
    std::string keyword = "";
    size_t pos = position;
    
    while (pos < source.length() && std::isalpha(source[pos])) {
        keyword += source[pos];
        pos++;
    }
    
    return keyword == "script";
}

CodeFragment UnifiedScanner::scanCHTLBlock() {
    size_t startLine = line;
    size_t startColumn = column;
    size_t startPos = position;
    
    std::string content = "";
    int braceLevel = 0;
    bool inString = false;
    char stringDelimiter = '\0';
    
    while (position < source.length()) {
        char c = currentChar();
        
        if (inString) {
            if (c == stringDelimiter) {
                inString = false;
                stringDelimiter = '\0';
            }
        } else {
            if (c == '"' || c == '\'') {
                inString = true;
                stringDelimiter = c;
            } else if (c == '{') {
                braceLevel++;
            } else if (c == '}') {
                braceLevel--;
                if (braceLevel == 0) {
                    content += c;
                    advance();
                    break;
                }
            }
        }
        
        content += c;
        advance();
    }
    
    size_t endLine = line;
    size_t endColumn = column;
    
    return CodeFragment{CodeType::CHTL, content, startLine, startColumn, endLine, endColumn};
}

CodeFragment UnifiedScanner::scanCHTLJSBlock() {
    size_t startLine = line;
    size_t startColumn = column;
    size_t startPos = position;
    
    std::string content = "";
    int braceLevel = 0;
    bool inString = false;
    char stringDelimiter = '\0';
    
    while (position < source.length()) {
        char c = currentChar();
        
        if (inString) {
            if (c == stringDelimiter) {
                inString = false;
                stringDelimiter = '\0';
            }
        } else {
            if (c == '"' || c == '\'') {
                inString = true;
                stringDelimiter = c;
            } else if (c == '{') {
                braceLevel++;
            } else if (c == '}') {
                braceLevel--;
                if (braceLevel == 0) {
                    content += c;
                    advance();
                    break;
                }
            }
        }
        
        content += c;
        advance();
    }
    
    size_t endLine = line;
    size_t endColumn = column;
    
    return CodeFragment{CodeType::CHTL_JS, content, startLine, startColumn, endLine, endColumn};
}

CodeFragment UnifiedScanner::scanCSSBlock() {
    size_t startLine = line;
    size_t startColumn = column;
    size_t startPos = position;
    
    std::string content = "";
    int braceLevel = 0;
    bool inString = false;
    char stringDelimiter = '\0';
    
    while (position < source.length()) {
        char c = currentChar();
        
        if (inString) {
            if (c == stringDelimiter) {
                inString = false;
                stringDelimiter = '\0';
            }
        } else {
            if (c == '"' || c == '\'') {
                inString = true;
                stringDelimiter = c;
            } else if (c == '{') {
                braceLevel++;
            } else if (c == '}') {
                braceLevel--;
                if (braceLevel == 0) {
                    content += c;
                    advance();
                    break;
                }
            }
        }
        
        content += c;
        advance();
    }
    
    size_t endLine = line;
    size_t endColumn = column;
    
    return CodeFragment{CodeType::CSS, content, startLine, startColumn, endLine, endColumn};
}

CodeFragment UnifiedScanner::scanJSBlock() {
    size_t startLine = line;
    size_t startColumn = column;
    size_t startPos = position;
    
    std::string content = "";
    int braceLevel = 0;
    bool inString = false;
    char stringDelimiter = '\0';
    
    while (position < source.length()) {
        char c = currentChar();
        
        if (inString) {
            if (c == stringDelimiter) {
                inString = false;
                stringDelimiter = '\0';
            }
        } else {
            if (c == '"' || c == '\'') {
                inString = true;
                stringDelimiter = c;
            } else if (c == '{') {
                braceLevel++;
            } else if (c == '}') {
                braceLevel--;
                if (braceLevel == 0) {
                    content += c;
                    advance();
                    break;
                }
            }
        }
        
        content += c;
        advance();
    }
    
    size_t endLine = line;
    size_t endColumn = column;
    
    return CodeFragment{CodeType::JS, content, startLine, startColumn, endLine, endColumn};
}

CodeFragment UnifiedScanner::scanHTMLBlock() {
    size_t startLine = line;
    size_t startColumn = column;
    size_t startPos = position;
    
    std::string content = "";
    int braceLevel = 0;
    bool inString = false;
    char stringDelimiter = '\0';
    
    while (position < source.length()) {
        char c = currentChar();
        
        if (inString) {
            if (c == stringDelimiter) {
                inString = false;
                stringDelimiter = '\0';
            }
        } else {
            if (c == '"' || c == '\'') {
                inString = true;
                stringDelimiter = c;
            } else if (c == '{') {
                braceLevel++;
            } else if (c == '}') {
                braceLevel--;
                if (braceLevel == 0) {
                    content += c;
                    advance();
                    break;
                }
            }
        }
        
        content += c;
        advance();
    }
    
    size_t endLine = line;
    size_t endColumn = column;
    
    return CodeFragment{CodeType::HTML, content, startLine, startColumn, endLine, endColumn};
}

CodeFragment UnifiedScanner::scanUnknownBlock() {
    size_t startLine = line;
    size_t startColumn = column;
    size_t startPos = position;
    
    std::string content = "";
    
    // 扫描到下一个已知块或文件结束
    while (position < source.length()) {
        char c = currentChar();
        content += c;
        advance();
        
        // 检查是否遇到已知块
        if (isCHTLStart() || isCHTLJSBlockStart() || isCSSStart() || isHTMLStart()) {
            break;
        }
    }
    
    size_t endLine = line;
    size_t endColumn = column;
    
    return CodeFragment{CodeType::UNKNOWN, content, startLine, startColumn, endLine, endColumn};
}

char UnifiedScanner::currentChar() const {
    if (position < source.length()) {
        return source[position];
    }
    return '\0';
}

char UnifiedScanner::peekChar(size_t offset) const {
    if (position + offset < source.length()) {
        return source[position + offset];
    }
    return '\0';
}

void UnifiedScanner::advance() {
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

void UnifiedScanner::skipWhitespace() {
    while (position < source.length() && std::isspace(currentChar())) {
        advance();
    }
}

void UnifiedScanner::skipNewline() {
    if (currentChar() == '\n') {
        advance();
    }
}

void UnifiedScanner::throwError(const std::string& message) const {
    throw std::runtime_error("UnifiedScanner Error at Line " + std::to_string(line) + 
                            ", Column " + std::to_string(column) + ": " + message);
}

void UnifiedScanner::synchronize() {
    // 同步到下一个安全点
    while (position < source.length()) {
        if (currentChar() == ';' || currentChar() == '}') {
            advance();
            break;
        }
        advance();
    }
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
    oss << "  Position: " << position << "\n";
    oss << "  Line: " << line << ", Column: " << column << "\n";
    oss << "  Total Fragments: " << stats.totalFragments << "\n";
    oss << "  CHTL Fragments: " << stats.chtlFragments << "\n";
    oss << "  CHTL JS Fragments: " << stats.chtlJSFragments << "\n";
    oss << "  CSS Fragments: " << stats.cssFragments << "\n";
    oss << "  JS Fragments: " << stats.jsFragments << "\n";
    oss << "  HTML Fragments: " << stats.htmlFragments << "\n";
    oss << "  Unknown Fragments: " << stats.unknownFragments << "\n";
    oss << "  Scan Time: " << stats.scanTime << "ms\n";
    return oss.str();
}

UnifiedScanner::ScanStats UnifiedScanner::getStats() const {
    return stats;
}

void UnifiedScanner::resetStats() {
    stats = ScanStats{};
}

std::string UnifiedScanner::createPlaceholder(CodeType type, size_t index) {
    std::string prefix;
    switch (type) {
        case CodeType::CHTL: prefix = "CHTL"; break;
        case CodeType::CHTL_JS: prefix = "CHTLJS"; break;
        case CodeType::CSS: prefix = "CSS"; break;
        case CodeType::JS: prefix = "JS"; break;
        case CodeType::HTML: prefix = "HTML"; break;
        default: prefix = "UNKNOWN"; break;
    }
    
    return "{{" + prefix + "_" + std::to_string(index) + "}}";
}

// 添加缺失的优化方法
bool UnifiedScanner::isSafeBoundary() const {
    // 检查当前位置是否为安全的代码边界
    if (position >= source.length()) {
        return true;
    }
    
    char c = currentChar();
    
    // 检查是否在字符串或注释中
    if (state.inString || state.inComment || state.inMultiLineComment) {
        return false;
    }
    
    // 检查括号层级
    if (state.braceLevel > 0 || state.parenLevel > 0 || state.bracketLevel > 0) {
        return false;
    }
    
    // 检查特殊关键字边界
    if (c == ';' || c == '}' || c == ')' || c == ']') {
        return true;
    }
    
    return false;
}

void UnifiedScanner::expandBoundary() {
    // 智能扩增边界，避免截断语法结构
    size_t originalPos = position;
    
    // 向前扫描寻找安全边界
    while (position < source.length() && !isSafeBoundary()) {
        char c = currentChar();
        
        // 更新状态
        if (c == '"' || c == '\'') {
            if (!state.inString) {
                state.inString = true;
                state.stringDelimiter = c;
            } else if (c == state.stringDelimiter) {
                state.inString = false;
                state.stringDelimiter = '\0';
            }
        } else if (c == '/' && peekChar(1) == '/') {
            // 单行注释
            while (position < source.length() && currentChar() != '\n') {
                advance();
            }
        } else if (c == '/' && peekChar(1) == '*') {
            // 多行注释
            state.inMultiLineComment = true;
            advance(); // 跳过 *
            while (position < source.length()) {
                if (currentChar() == '*' && peekChar(1) == '/') {
                    advance(); // 跳过 *
                    advance(); // 跳过 /
                    state.inMultiLineComment = false;
                    break;
                }
                advance();
            }
        } else if (c == '{') {
            state.braceLevel++;
        } else if (c == '}') {
            state.braceLevel--;
        } else if (c == '(') {
            state.parenLevel++;
        } else if (c == ')') {
            state.parenLevel--;
        } else if (c == '[') {
            state.bracketLevel++;
        } else if (c == ']') {
            state.bracketLevel--;
        }
        
        advance();
    }
}

void UnifiedScanner::contractBoundary() {
    // 回退边界，用于错误恢复
    // 实现回退逻辑
}

bool UnifiedScanner::isWideJudge() const {
    // 宽判：用于处理大块CHTL代码
    return !state.inString && !state.inComment && !state.inMultiLineComment;
}

bool UnifiedScanner::isStrictJudge() const {
    // 严判：用于处理CHTL JS和JS混合代码
    return state.inCHTLJSBlock || (state.braceLevel > 0 && !state.inString);
}

void UnifiedScanner::registerPlaceholder(const std::string& placeholder, const std::string& content) {
    placeholders[placeholder] = content;
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

} // namespace CHTL