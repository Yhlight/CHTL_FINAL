#include "UnifiedScanner.h"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace CHTL {

UnifiedScanner::UnifiedScanner() : debug_mode_(false) {
}

UnifiedScanner::~UnifiedScanner() {
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::scan(const std::string& source) {
    std::vector<CodeFragment> fragments;
    size_t pos = 0;
    
    while (pos < source.length()) {
        // 跳过空白字符
        while (pos < source.length() && isWhitespace(source[pos])) {
            pos++;
        }
        
        if (pos >= source.length()) {
            break;
        }
        
        // 检测代码类型
        CodeType type = CodeType::UNKNOWN;
        CodeFragment fragment;
        
        if (isCHTLBoundary(source, pos)) {
            fragment = scanCHTLBlock(source, pos);
            type = CodeType::CHTL;
        } else if (isCHTLJSBoundary(source, pos)) {
            fragment = scanCHTLJSBlock(source, pos);
            type = CodeType::CHTL_JS;
        } else if (isCSSBoundary(source, pos)) {
            fragment = scanCSSBlock(source, pos);
            type = CodeType::CSS;
        } else if (isJSBoundary(source, pos)) {
            fragment = scanJSBlock(source, pos);
            type = CodeType::JS;
        } else {
            // 默认为JS代码
            fragment = scanJSBlock(source, pos);
            type = CodeType::JS;
        }
        
        if (fragment.type != CodeType::UNKNOWN) {
            fragments.push_back(fragment);
            pos = fragment.end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::scanCHTL(const std::string& source) {
    std::vector<CodeFragment> fragments;
    size_t pos = 0;
    
    while (pos < source.length()) {
        if (isCHTLBoundary(source, pos)) {
            CodeFragment fragment = scanCHTLBlock(source, pos);
            if (fragment.type == CodeType::CHTL) {
                fragments.push_back(fragment);
            }
            pos = fragment.end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::scanCHTLJS(const std::string& source) {
    std::vector<CodeFragment> fragments;
    size_t pos = 0;
    
    while (pos < source.length()) {
        if (isCHTLJSBoundary(source, pos)) {
            CodeFragment fragment = scanCHTLJSBlock(source, pos);
            if (fragment.type == CodeType::CHTL_JS) {
                fragments.push_back(fragment);
            }
            pos = fragment.end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::scanCSS(const std::string& source) {
    std::vector<CodeFragment> fragments;
    size_t pos = 0;
    
    while (pos < source.length()) {
        if (isCSSBoundary(source, pos)) {
            CodeFragment fragment = scanCSSBlock(source, pos);
            if (fragment.type == CodeType::CSS) {
                fragments.push_back(fragment);
            }
            pos = fragment.end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::scanJS(const std::string& source) {
    std::vector<CodeFragment> fragments;
    size_t pos = 0;
    
    while (pos < source.length()) {
        if (isJSBoundary(source, pos)) {
            CodeFragment fragment = scanJSBlock(source, pos);
            if (fragment.type == CodeType::JS) {
                fragments.push_back(fragment);
            }
            pos = fragment.end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

void UnifiedScanner::setDebugMode(bool debug) {
    debug_mode_ = debug;
}

bool UnifiedScanner::isDebugMode() const {
    return debug_mode_;
}

void UnifiedScanner::addPlaceholder(const std::string& placeholder, const std::string& content) {
    placeholders_[placeholder] = content;
}

std::string UnifiedScanner::getPlaceholder(const std::string& placeholder) const {
    auto it = placeholders_.find(placeholder);
    if (it != placeholders_.end()) {
        return it->second;
    }
    return "";
}

void UnifiedScanner::clearPlaceholders() {
    placeholders_.clear();
}

void UnifiedScanner::addError(const std::string& error) {
    errors_.push_back(error);
    if (debug_mode_) {
        std::cerr << "[UnifiedScanner ERROR] " << error << std::endl;
    }
}

const std::vector<std::string>& UnifiedScanner::getErrors() const {
    return errors_;
}

bool UnifiedScanner::hasErrors() const {
    return !errors_.empty();
}

void UnifiedScanner::clearErrors() {
    errors_.clear();
}

UnifiedScanner::CodeFragment UnifiedScanner::scanCHTLBlock(const std::string& source, size_t start_pos) {
    CodeFragment fragment;
    fragment.type = CodeType::CHTL;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    size_t pos = start_pos;
    
    // 检测CHTL语法边界
    if (isTemplateSyntax(source, pos) || 
        isCustomSyntax(source, pos) || 
        isOriginSyntax(source, pos) || 
        isImportSyntax(source, pos) || 
        isNamespaceSyntax(source, pos) || 
        isConfigurationSyntax(source, pos)) {
        
        // 找到匹配的右括号
        size_t end_pos = findMatchingBrace(source, pos);
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 1;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
        } else {
            fragment.type = CodeType::UNKNOWN;
            fragment.end_pos = pos + 1;
            addError("Unmatched brace in CHTL block at line " + std::to_string(fragment.line));
        }
    } else {
        // 检测HTML元素
        while (pos < source.length() && !isWhitespace(source[pos]) && source[pos] != '{') {
            pos++;
        }
        
        if (pos < source.length() && source[pos] == '{') {
            size_t end_pos = findMatchingBrace(source, pos);
            if (end_pos != std::string::npos) {
                fragment.end_pos = end_pos + 1;
                fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
            } else {
                fragment.type = CodeType::UNKNOWN;
                fragment.end_pos = pos + 1;
                addError("Unmatched brace in CHTL element at line " + std::to_string(fragment.line));
            }
        } else {
            fragment.type = CodeType::UNKNOWN;
            fragment.end_pos = pos;
        }
    }
    
    return fragment;
}

UnifiedScanner::CodeFragment UnifiedScanner::scanCHTLJSBlock(const std::string& source, size_t start_pos) {
    CodeFragment fragment;
    fragment.type = CodeType::CHTL_JS;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    size_t pos = start_pos;
    
    // 检测CHTL JS关键字
    if (isCHTLJSKeyword(source, pos)) {
        // 找到匹配的右括号
        size_t end_pos = findMatchingBrace(source, pos);
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 1;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
        } else {
            fragment.type = CodeType::UNKNOWN;
            fragment.end_pos = pos + 1;
            addError("Unmatched brace in CHTL JS block at line " + std::to_string(fragment.line));
        }
    } else if (isSelectorSyntax(source, pos) || isResponsiveValueSyntax(source, pos)) {
        // 处理选择器或响应式值
        while (pos < source.length() && source[pos] != '}') {
            pos++;
        }
        if (pos < source.length()) {
            pos++; // 包含右括号
        }
        fragment.end_pos = pos;
        fragment.content = source.substr(start_pos, pos - start_pos);
    } else {
        fragment.type = CodeType::UNKNOWN;
        fragment.end_pos = pos + 1;
    }
    
    return fragment;
}

UnifiedScanner::CodeFragment UnifiedScanner::scanCSSBlock(const std::string& source, size_t start_pos) {
    CodeFragment fragment;
    fragment.type = CodeType::CSS;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    size_t pos = start_pos;
    
    // 检测CSS选择器
    while (pos < source.length() && source[pos] != '{') {
        pos++;
    }
    
    if (pos < source.length() && source[pos] == '{') {
        size_t end_pos = findMatchingBrace(source, pos);
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 1;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
        } else {
            fragment.type = CodeType::UNKNOWN;
            fragment.end_pos = pos + 1;
            addError("Unmatched brace in CSS block at line " + std::to_string(fragment.line));
        }
    } else {
        fragment.type = CodeType::UNKNOWN;
        fragment.end_pos = pos;
    }
    
    return fragment;
}

UnifiedScanner::CodeFragment UnifiedScanner::scanJSBlock(const std::string& source, size_t start_pos) {
    CodeFragment fragment;
    fragment.type = CodeType::JS;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    size_t pos = start_pos;
    
    // 检测JS函数、对象或语句
    if (source[pos] == '{') {
        size_t end_pos = findMatchingBrace(source, pos);
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 1;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
        } else {
            fragment.type = CodeType::UNKNOWN;
            fragment.end_pos = pos + 1;
            addError("Unmatched brace in JS block at line " + std::to_string(fragment.line));
        }
    } else if (source[pos] == '(') {
        size_t end_pos = findMatchingParen(source, pos);
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 1;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
        } else {
            fragment.type = CodeType::UNKNOWN;
            fragment.end_pos = pos + 1;
            addError("Unmatched parenthesis in JS block at line " + std::to_string(fragment.line));
        }
    } else if (source[pos] == '[') {
        size_t end_pos = findMatchingBracket(source, pos);
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 1;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
        } else {
            fragment.type = CodeType::UNKNOWN;
            fragment.end_pos = pos + 1;
            addError("Unmatched bracket in JS block at line " + std::to_string(fragment.line));
        }
    } else {
        // 单行语句
        while (pos < source.length() && source[pos] != ';' && source[pos] != '\n') {
            pos++;
        }
        if (pos < source.length() && source[pos] == ';') {
            pos++; // 包含分号
        }
        fragment.end_pos = pos;
        fragment.content = source.substr(start_pos, pos - start_pos);
    }
    
    return fragment;
}

bool UnifiedScanner::isCHTLBoundary(const std::string& source, size_t pos) {
    return isTemplateSyntax(source, pos) || 
           isCustomSyntax(source, pos) || 
           isOriginSyntax(source, pos) || 
           isImportSyntax(source, pos) || 
           isNamespaceSyntax(source, pos) || 
           isConfigurationSyntax(source, pos) ||
           isCHTLKeyword(source, pos);
}

bool UnifiedScanner::isCHTLJSBoundary(const std::string& source, size_t pos) {
    return isCHTLJSKeyword(source, pos) || 
           isSelectorSyntax(source, pos) || 
           isResponsiveValueSyntax(source, pos);
}

bool UnifiedScanner::isCSSBoundary(const std::string& source, size_t pos) {
    return isCSSKeyword(source, pos);
}

bool UnifiedScanner::isJSBoundary(const std::string& source, size_t pos) {
    return isJSKeyword(source, pos) || 
           source[pos] == '{' || 
           source[pos] == '(' || 
           source[pos] == '[';
}

size_t UnifiedScanner::findMatchingBrace(const std::string& source, size_t start_pos) {
    int brace_count = 0;
    size_t pos = start_pos;
    
    while (pos < source.length()) {
        if (source[pos] == '{') {
            brace_count++;
        } else if (source[pos] == '}') {
            brace_count--;
            if (brace_count == 0) {
                return pos;
            }
        }
        pos++;
    }
    
    return std::string::npos;
}

size_t UnifiedScanner::findMatchingParen(const std::string& source, size_t start_pos) {
    int paren_count = 0;
    size_t pos = start_pos;
    
    while (pos < source.length()) {
        if (source[pos] == '(') {
            paren_count++;
        } else if (source[pos] == ')') {
            paren_count--;
            if (paren_count == 0) {
                return pos;
            }
        }
        pos++;
    }
    
    return std::string::npos;
}

size_t UnifiedScanner::findMatchingBracket(const std::string& source, size_t start_pos) {
    int bracket_count = 0;
    size_t pos = start_pos;
    
    while (pos < source.length()) {
        if (source[pos] == '[') {
            bracket_count++;
        } else if (source[pos] == ']') {
            bracket_count--;
            if (bracket_count == 0) {
                return pos;
            }
        }
        pos++;
    }
    
    return std::string::npos;
}

bool UnifiedScanner::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool UnifiedScanner::isNewline(char c) {
    return c == '\n' || c == '\r';
}

int UnifiedScanner::getLineNumber(const std::string& source, size_t pos) {
    int line = 1;
    for (size_t i = 0; i < pos && i < source.length(); i++) {
        if (source[i] == '\n') {
            line++;
        }
    }
    return line;
}

int UnifiedScanner::getColumnNumber(const std::string& source, size_t pos) {
    int column = 1;
    size_t line_start = pos;
    
    // 找到当前行的开始位置
    while (line_start > 0 && source[line_start - 1] != '\n') {
        line_start--;
    }
    
    return pos - line_start + 1;
}

bool UnifiedScanner::isCHTLKeyword(const std::string& source, size_t pos) {
    // 检测HTML元素
    if (pos < source.length() && std::isalpha(source[pos])) {
        size_t end = pos;
        while (end < source.length() && (std::isalnum(source[end]) || source[end] == '-')) {
            end++;
        }
        if (end < source.length() && source[end] == '{') {
            return true;
        }
    }
    return false;
}

bool UnifiedScanner::isCHTLJSKeyword(const std::string& source, size_t pos) {
    static const std::vector<std::string> keywords = {
        "ScriptLoader", "Listen", "Animate", "Router", "Vir", 
        "iNeverAway", "util", "printMylove"
    };
    
    for (const std::string& keyword : keywords) {
        if (pos + keyword.length() <= source.length() && 
            source.substr(pos, keyword.length()) == keyword) {
            return true;
        }
    }
    return false;
}

bool UnifiedScanner::isCSSKeyword(const std::string& source, size_t pos) {
    // 检测CSS选择器
    if (pos < source.length() && (source[pos] == '.' || source[pos] == '#' || std::isalpha(source[pos]))) {
        size_t end = pos;
        while (end < source.length() && source[end] != '{' && source[end] != ';') {
            end++;
        }
        if (end < source.length() && source[end] == '{') {
            return true;
        }
    }
    return false;
}

bool UnifiedScanner::isJSKeyword(const std::string& source, size_t pos) {
    static const std::vector<std::string> keywords = {
        "function", "var", "let", "const", "if", "else", "for", "while", 
        "do", "switch", "case", "default", "break", "continue", "return", 
        "try", "catch", "finally", "throw", "new", "this", "typeof", 
        "instanceof", "in", "of", "class", "extends", "import", "export"
    };
    
    for (const std::string& keyword : keywords) {
        if (pos + keyword.length() <= source.length() && 
            source.substr(pos, keyword.length()) == keyword) {
            return true;
        }
    }
    return false;
}

bool UnifiedScanner::isSelectorSyntax(const std::string& source, size_t pos) {
    return pos + 1 < source.length() && source[pos] == '{' && source[pos + 1] == '{';
}

bool UnifiedScanner::isResponsiveValueSyntax(const std::string& source, size_t pos) {
    return pos < source.length() && source[pos] == '$';
}

bool UnifiedScanner::isTemplateSyntax(const std::string& source, size_t pos) {
    return pos + 8 < source.length() && source.substr(pos, 8) == "[Template]";
}

bool UnifiedScanner::isCustomSyntax(const std::string& source, size_t pos) {
    return pos + 7 < source.length() && source.substr(pos, 7) == "[Custom]";
}

bool UnifiedScanner::isOriginSyntax(const std::string& source, size_t pos) {
    return pos + 7 < source.length() && source.substr(pos, 7) == "[Origin]";
}

bool UnifiedScanner::isImportSyntax(const std::string& source, size_t pos) {
    return pos + 7 < source.length() && source.substr(pos, 7) == "[Import]";
}

bool UnifiedScanner::isNamespaceSyntax(const std::string& source, size_t pos) {
    return pos + 10 < source.length() && source.substr(pos, 10) == "[Namespace]";
}

bool UnifiedScanner::isConfigurationSyntax(const std::string& source, size_t pos) {
    return pos + 13 < source.length() && source.substr(pos, 13) == "[Configuration]";
}

} // namespace CHTL