#include "UnifiedScanner.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace CHTL {

UnifiedScanner::UnifiedScanner() 
    : placeholderCounter(0), position(0), line(1), column(1),
      debugMode(false), preserveComments(true), enablePlaceholders(true) {}

void UnifiedScanner::scanSource() {
    fragments.clear();
    placeholders.clear();
    position = 0;
    line = 1;
    column = 1;
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        // 检测代码类型并扫描
        if (isCHTLBoundary()) {
            auto fragment = scanCHTL();
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
            }
        } else if (isCHTLJSBoundary()) {
            auto fragment = scanCHTLJS();
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
            }
        } else if (isCSSBoundary()) {
            auto fragment = scanCSS();
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
            }
        } else if (isJSBoundary()) {
            auto fragment = scanJS();
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
            }
        } else if (isHTMLBoundary()) {
            auto fragment = scanHTML();
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
            }
        } else {
            // 默认作为CHTL处理
            auto fragment = scanCHTL();
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
            }
        }
    }
}

CodeFragment UnifiedScanner::scanCHTL() {
    CodeFragment fragment(CodeType::CHTL);
    fragment.startLine = line;
    fragment.startColumn = column;
    
    size_t braceCount = 0;
    bool inString = false;
    char stringChar = 0;
    
    while (!isAtEnd()) {
        char c = current();
        
        if (!inString) {
            if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            } else if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                if (braceCount > 0) {
                    braceCount--;
                } else {
                    // 结束CHTL块
                    fragment.content += c;
                    advance();
                    break;
                }
            } else if (c == '[' && peek() == 'T') {
                // 检测到[Template]等特殊标记
                std::string marker;
                size_t tempPos = position;
                while (tempPos < source.length() && source[tempPos] != ']') {
                    marker += source[tempPos];
                    tempPos++;
                }
                if (tempPos < source.length()) {
                    marker += ']';
                    if (marker == "[Template]" || marker == "[Custom]" || 
                        marker == "[Origin]" || marker == "[Import]" ||
                        marker == "[Namespace]" || marker == "[Configuration]" ||
                        marker == "[Info]" || marker == "[Export]") {
                        // 这是CHTL的特殊语法，继续扫描
                    }
                }
            }
        } else {
            if (c == stringChar && (position == 0 || source[position - 1] != '\\')) {
                inString = false;
            }
        }
        
        fragment.content += c;
        advance();
        
        // 检查是否遇到其他代码类型的边界
        if (!inString && braceCount == 0) {
            if (isCHTLJSBoundary() || isCSSBoundary() || isJSBoundary() || isHTMLBoundary()) {
                break;
            }
        }
    }
    
    fragment.endLine = line;
    fragment.endColumn = column;
    
    if (enablePlaceholders) {
        fragment.placeholder = generatePlaceholder();
        addPlaceholder(fragment.placeholder, fragment);
    }
    
    return fragment;
}

CodeFragment UnifiedScanner::scanCHTLJS() {
    CodeFragment fragment(CodeType::CHTLJS);
    fragment.startLine = line;
    fragment.startColumn = column;
    
    // CHTL JS通常出现在script块中
    while (!isAtEnd()) {
        char c = current();
        
        // 检查是否遇到CHTL JS的结束边界
        if (c == '}' && peek() == '}') {
            // 这是CHTL的结束，不是CHTL JS
            break;
        }
        
        fragment.content += c;
        advance();
        
        // 检查是否遇到其他代码类型的边界
        if (isCHTLBoundary() || isCSSBoundary() || isJSBoundary() || isHTMLBoundary()) {
            break;
        }
    }
    
    fragment.endLine = line;
    fragment.endColumn = column;
    
    if (enablePlaceholders) {
        fragment.placeholder = generatePlaceholder();
        addPlaceholder(fragment.placeholder, fragment);
    }
    
    return fragment;
}

CodeFragment UnifiedScanner::scanCSS() {
    CodeFragment fragment(CodeType::CSS);
    fragment.startLine = line;
    fragment.startColumn = column;
    
    size_t braceCount = 0;
    bool inString = false;
    char stringChar = 0;
    bool inComment = false;
    
    while (!isAtEnd()) {
        char c = current();
        
        if (!inComment) {
            if (c == '/' && peek() == '*') {
                inComment = true;
                fragment.content += c;
                advance();
                fragment.content += current();
                advance();
                continue;
            }
            
            if (!inString) {
                if (c == '"' || c == '\'') {
                    inString = true;
                    stringChar = c;
                } else if (c == '{') {
                    braceCount++;
                } else if (c == '}') {
                    if (braceCount > 0) {
                        braceCount--;
                    } else {
                        // 结束CSS块
                        fragment.content += c;
                        advance();
                        break;
                    }
                }
            } else {
                if (c == stringChar && (position == 0 || source[position - 1] != '\\')) {
                    inString = false;
                }
            }
        } else {
            if (c == '*' && peek() == '/') {
                inComment = false;
                fragment.content += c;
                advance();
                fragment.content += current();
                advance();
                continue;
            }
        }
        
        fragment.content += c;
        advance();
        
        // 检查是否遇到其他代码类型的边界
        if (!inComment && !inString && braceCount == 0) {
            if (isCHTLBoundary() || isCHTLJSBoundary() || isJSBoundary() || isHTMLBoundary()) {
                break;
            }
        }
    }
    
    fragment.endLine = line;
    fragment.endColumn = column;
    
    if (enablePlaceholders) {
        fragment.placeholder = generatePlaceholder();
        addPlaceholder(fragment.placeholder, fragment);
    }
    
    return fragment;
}

CodeFragment UnifiedScanner::scanJS() {
    CodeFragment fragment(CodeType::JS);
    fragment.startLine = line;
    fragment.startColumn = column;
    
    size_t braceCount = 0;
    size_t parenCount = 0;
    size_t bracketCount = 0;
    bool inString = false;
    char stringChar = 0;
    bool inComment = false;
    bool inLineComment = false;
    
    while (!isAtEnd()) {
        char c = current();
        
        if (!inComment && !inLineComment) {
            if (c == '/' && peek() == '/') {
                inLineComment = true;
                fragment.content += c;
                advance();
                fragment.content += current();
                advance();
                continue;
            }
            
            if (c == '/' && peek() == '*') {
                inComment = true;
                fragment.content += c;
                advance();
                fragment.content += current();
                advance();
                continue;
            }
            
            if (!inString) {
                if (c == '"' || c == '\'') {
                    inString = true;
                    stringChar = c;
                } else if (c == '{') {
                    braceCount++;
                } else if (c == '}') {
                    if (braceCount > 0) {
                        braceCount--;
                    }
                } else if (c == '(') {
                    parenCount++;
                } else if (c == ')') {
                    if (parenCount > 0) {
                        parenCount--;
                    }
                } else if (c == '[') {
                    bracketCount++;
                } else if (c == ']') {
                    if (bracketCount > 0) {
                        bracketCount--;
                    }
                }
            } else {
                if (c == stringChar && (position == 0 || source[position - 1] != '\\')) {
                    inString = false;
                }
            }
        } else {
            if (inLineComment && c == '\n') {
                inLineComment = false;
            } else if (inComment && c == '*' && peek() == '/') {
                inComment = false;
                fragment.content += c;
                advance();
                fragment.content += current();
                advance();
                continue;
            }
        }
        
        fragment.content += c;
        advance();
        
        // 检查是否遇到其他代码类型的边界
        if (!inComment && !inLineComment && !inString && 
            braceCount == 0 && parenCount == 0 && bracketCount == 0) {
            if (isCHTLBoundary() || isCHTLJSBoundary() || isCSSBoundary() || isHTMLBoundary()) {
                break;
            }
        }
    }
    
    fragment.endLine = line;
    fragment.endColumn = column;
    
    if (enablePlaceholders) {
        fragment.placeholder = generatePlaceholder();
        addPlaceholder(fragment.placeholder, fragment);
    }
    
    return fragment;
}

CodeFragment UnifiedScanner::scanHTML() {
    CodeFragment fragment(CodeType::HTML);
    fragment.startLine = line;
    fragment.startColumn = column;
    
    bool inTag = false;
    bool inString = false;
    char stringChar = 0;
    
    while (!isAtEnd()) {
        char c = current();
        
        if (!inString) {
            if (c == '<') {
                inTag = true;
            } else if (c == '>') {
                inTag = false;
            } else if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            }
        } else {
            if (c == stringChar && (position == 0 || source[position - 1] != '\\')) {
                inString = false;
            }
        }
        
        fragment.content += c;
        advance();
        
        // 检查是否遇到其他代码类型的边界
        if (!inTag && !inString) {
            if (isCHTLBoundary() || isCHTLJSBoundary() || isCSSBoundary() || isJSBoundary()) {
                break;
            }
        }
    }
    
    fragment.endLine = line;
    fragment.endColumn = column;
    
    if (enablePlaceholders) {
        fragment.placeholder = generatePlaceholder();
        addPlaceholder(fragment.placeholder, fragment);
    }
    
    return fragment;
}

char UnifiedScanner::current() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char UnifiedScanner::peek(size_t offset) const {
    size_t index = position + offset;
    if (index >= source.length()) {
        return '\0';
    }
    return source[index];
}

void UnifiedScanner::advance() {
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

bool UnifiedScanner::isAtEnd() const {
    return position >= source.length();
}

void UnifiedScanner::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(current())) {
        advance();
    }
}

void UnifiedScanner::skipComment() {
    if (current() == '/' && peek() == '/') {
        skipLineComment();
    } else if (current() == '/' && peek() == '*') {
        skipBlockComment();
    }
}

void UnifiedScanner::skipLineComment() {
    while (!isAtEnd() && !isNewline(current())) {
        advance();
    }
}

void UnifiedScanner::skipBlockComment() {
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

bool UnifiedScanner::isCHTLBoundary() const {
    // 检测CHTL语法边界
    if (current() == '[') {
        std::string marker;
        size_t tempPos = position;
        while (tempPos < source.length() && source[tempPos] != ']') {
            marker += source[tempPos];
            tempPos++;
        }
        if (tempPos < source.length()) {
            marker += ']';
            return marker == "[Template]" || marker == "[Custom]" || 
                   marker == "[Origin]" || marker == "[Import]" ||
                   marker == "[Namespace]" || marker == "[Configuration]" ||
                   marker == "[Info]" || marker == "[Export]";
        }
    }
    
    // 检测HTML元素
    if (isAlpha(current())) {
        std::string element;
        size_t tempPos = position;
        while (tempPos < source.length() && isAlphaNumeric(source[tempPos])) {
            element += source[tempPos];
            tempPos++;
        }
        
        // 常见的HTML元素
        static const std::vector<std::string> htmlElements = {
            "html", "head", "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
            "a", "img", "ul", "ol", "li", "table", "tr", "td", "th", "form", "input",
            "button", "textarea", "select", "option", "label", "fieldset", "legend"
        };
        
        return std::find(htmlElements.begin(), htmlElements.end(), element) != htmlElements.end();
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSBoundary() const {
    // 检测CHTL JS语法边界
    if (current() == '{' && peek() == '{') {
        return true;
    }
    
    // 检测CHTL JS关键字
    if (isAlpha(current())) {
        std::string keyword;
        size_t tempPos = position;
        while (tempPos < source.length() && isAlphaNumeric(source[tempPos])) {
            keyword += source[tempPos];
            tempPos++;
        }
        
        static const std::vector<std::string> chtljsKeywords = {
            "Vir", "Listen", "Animate", "Router", "ScriptLoader", "Delegate"
        };
        
        return std::find(chtljsKeywords.begin(), chtljsKeywords.end(), keyword) != chtljsKeywords.end();
    }
    
    return false;
}

bool UnifiedScanner::isCSSBoundary() const {
    // 检测CSS语法边界
    if (current() == '.' || current() == '#' || current() == '@') {
        return true;
    }
    
    // 检测CSS选择器
    if (isAlpha(current()) || current() == '*') {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isJSBoundary() const {
    // 检测JavaScript语法边界
    if (current() == 'f' && peek() == 'u' && peek(2) == 'n' && peek(3) == 'c' && peek(4) == 't' && peek(5) == 'i' && peek(6) == 'o' && peek(7) == 'n') {
        return true;
    }
    
    if (current() == 'v' && peek() == 'a' && peek(2) == 'r') {
        return true;
    }
    
    if (current() == 'l' && peek() == 'e' && peek(2) == 't') {
        return true;
    }
    
    if (current() == 'c' && peek() == 'o' && peek(2) == 'n' && peek(3) == 's' && peek(4) == 't') {
        return true;
    }
    
    if (current() == 'i' && peek() == 'f') {
        return true;
    }
    
    if (current() == 'f' && peek() == 'o' && peek(2) == 'r') {
        return true;
    }
    
    if (current() == 'w' && peek() == 'h' && peek(2) == 'i' && peek(3) == 'l' && peek(4) == 'e') {
        return true;
    }
    
    if (current() == 'r' && peek() == 'e' && peek(2) == 't' && peek(3) == 'u' && peek(4) == 'r' && peek(5) == 'n') {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isHTMLBoundary() const {
    // 检测HTML语法边界
    if (current() == '<') {
        return true;
    }
    
    return false;
}

std::string UnifiedScanner::generatePlaceholder() {
    return "_CODE_PLACEHOLDER_" + std::to_string(placeholderCounter++);
}

void UnifiedScanner::addPlaceholder(const std::string& placeholder, const CodeFragment& fragment) {
    placeholders[placeholder] = fragment;
}

CodeFragment UnifiedScanner::getPlaceholder(const std::string& placeholder) const {
    auto it = placeholders.find(placeholder);
    if (it != placeholders.end()) {
        return it->second;
    }
    return CodeFragment();
}

bool UnifiedScanner::hasPlaceholder(const std::string& placeholder) const {
    return placeholders.find(placeholder) != placeholders.end();
}

bool UnifiedScanner::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool UnifiedScanner::isNewline(char c) const {
    return c == '\n';
}

bool UnifiedScanner::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool UnifiedScanner::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool UnifiedScanner::isDigit(char c) const {
    return std::isdigit(c);
}

void UnifiedScanner::error(const std::string& message) const {
    std::ostringstream oss;
    oss << "Scanner error at line " << line << ", column " << column << ": " << message;
    throw std::runtime_error(oss.str());
}

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& source) {
    this->source = source;
    scanSource();
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::scanFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    return scan(content);
}

void UnifiedScanner::setDebugMode(bool debug) {
    debugMode = debug;
}

void UnifiedScanner::setPreserveComments(bool preserve) {
    preserveComments = preserve;
}

void UnifiedScanner::setEnablePlaceholders(bool enable) {
    enablePlaceholders = enable;
}

bool UnifiedScanner::isDebugMode() const {
    return debugMode;
}

bool UnifiedScanner::isPreserveComments() const {
    return preserveComments;
}

bool UnifiedScanner::isEnablePlaceholders() const {
    return enablePlaceholders;
}

std::vector<CodeFragment> UnifiedScanner::getFragments() const {
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::getFragmentsByType(CodeType type) const {
    std::vector<CodeFragment> result;
    for (const auto& fragment : fragments) {
        if (fragment.type == type) {
            result.push_back(fragment);
        }
    }
    return result;
}

CodeFragment UnifiedScanner::getFragment(size_t index) const {
    if (index < fragments.size()) {
        return fragments[index];
    }
    return CodeFragment();
}

size_t UnifiedScanner::getFragmentCount() const {
    return fragments.size();
}

std::unordered_map<std::string, CodeFragment> UnifiedScanner::getPlaceholders() const {
    return placeholders;
}

std::string UnifiedScanner::restoreFromPlaceholders(const std::string& content) const {
    std::string result = content;
    for (const auto& pair : placeholders) {
        size_t pos = 0;
        while ((pos = result.find(pair.first, pos)) != std::string::npos) {
            result.replace(pos, pair.first.length(), pair.second.content);
            pos += pair.second.content.length();
        }
    }
    return result;
}

void UnifiedScanner::clear() {
    fragments.clear();
    placeholders.clear();
    placeholderCounter = 0;
    source.clear();
}

void UnifiedScanner::reset() {
    clear();
    position = 0;
    line = 1;
    column = 1;
}

void UnifiedScanner::printFragments() const {
    std::cout << "=== Code Fragments ===" << std::endl;
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        std::cout << "Fragment " << i << ":" << std::endl;
        std::cout << "  Type: " << static_cast<int>(fragment.type) << std::endl;
        std::cout << "  Content: " << fragment.content.substr(0, 100) << (fragment.content.length() > 100 ? "..." : "") << std::endl;
        std::cout << "  Lines: " << fragment.startLine << "-" << fragment.endLine << std::endl;
        std::cout << "  Placeholder: " << fragment.placeholder << std::endl;
        std::cout << std::endl;
    }
}

void UnifiedScanner::printPlaceholders() const {
    std::cout << "=== Placeholders ===" << std::endl;
    for (const auto& pair : placeholders) {
        std::cout << pair.first << " -> " << pair.second.content.substr(0, 50) << "..." << std::endl;
    }
}

void UnifiedScanner::printStatistics() const {
    std::cout << "=== Scanner Statistics ===" << std::endl;
    std::cout << "Total fragments: " << fragments.size() << std::endl;
    
    std::unordered_map<CodeType, int> typeCount;
    for (const auto& fragment : fragments) {
        typeCount[fragment.type]++;
    }
    
    for (const auto& pair : typeCount) {
        std::cout << "Type " << static_cast<int>(pair.first) << ": " << pair.second << " fragments" << std::endl;
    }
    
    std::cout << "Placeholders: " << placeholders.size() << std::endl;
}

} // namespace CHTL