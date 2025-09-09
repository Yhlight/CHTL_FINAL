#include "Scanner/CHTLUnifiedScanner.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>
#include <stack>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner() 
    : m_debugMode(false), m_strictMode(false), m_preserveComments(true),
      m_fragmentCount(0), m_chtlFragmentCount(0), m_chtljsFragmentCount(0),
      m_cssFragmentCount(0), m_javascriptFragmentCount(0) {
}

CHTLUnifiedScanner::~CHTLUnifiedScanner() = default;

std::vector<CodeFragment> CHTLUnifiedScanner::scan(const std::string& source) {
    return scanSource(source);
}

void CHTLUnifiedScanner::setDebugMode(bool enabled) {
    m_debugMode = enabled;
}

void CHTLUnifiedScanner::setStrictMode(bool enabled) {
    m_strictMode = enabled;
}

void CHTLUnifiedScanner::setPreserveComments(bool enabled) {
    m_preserveComments = enabled;
}

size_t CHTLUnifiedScanner::getFragmentCount() const {
    return m_fragmentCount;
}

size_t CHTLUnifiedScanner::getCHTLFragmentCount() const {
    return m_chtlFragmentCount;
}

size_t CHTLUnifiedScanner::getCHTLJSFragmentCount() const {
    return m_chtljsFragmentCount;
}

size_t CHTLUnifiedScanner::getCSSFragmentCount() const {
    return m_cssFragmentCount;
}

size_t CHTLUnifiedScanner::getJavaScriptFragmentCount() const {
    return m_javascriptFragmentCount;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scanSource(const std::string& source) {
    std::vector<CodeFragment> fragments;
    ScanState state;
    state.position = 0;
    state.line = 1;
    state.column = 1;
    state.inString = false;
    state.inComment = false;
    state.inCHTLBlock = false;
    state.inCHTLJSBlock = false;
    state.inCSSBlock = false;
    state.inJavaScriptBlock = false;
    state.braceLevel = 0;
    state.bracketLevel = 0;
    state.parenLevel = 0;
    state.currentType = CodeFragmentType::CHTL;
    
    scanFragment(state, source, fragments);
    
    m_fragmentCount = fragments.size();
    return fragments;
}

void CHTLUnifiedScanner::scanFragment(ScanState& state, const std::string& source, std::vector<CodeFragment>& fragments) {
    std::string currentFragment;
    size_t fragmentStart = state.position;
    size_t fragmentLine = state.line;
    size_t fragmentColumn = state.column;
    
    while (state.position < source.length()) {
        char c = getChar(source, state.position);
        
        // 更新行号和列号
        if (c == '\n') {
            state.line++;
            state.column = 1;
        } else {
            state.column++;
        }
        
        // 检查是否在字符串内
        if (c == '"' || c == '\'') {
            if (!state.inString) {
                state.inString = true;
                state.stringChar = c;
            } else if (c == state.stringChar) {
                state.inString = false;
            }
        }
        
        // 检查是否在注释内
        if (!state.inString) {
            if (c == '/' && state.position + 1 < source.length() && source[state.position + 1] == '/') {
                state.inComment = true;
            } else if (c == '\n' && state.inComment) {
                state.inComment = false;
            } else if (c == '/' && state.position + 1 < source.length() && source[state.position + 1] == '*') {
                state.inBlockComment = true;
            } else if (c == '*' && state.position + 1 < source.length() && source[state.position + 1] == '/' && state.inBlockComment) {
                state.inBlockComment = false;
                state.position++; // 跳过 '/'
                state.column++;
            }
        }
        
        // 如果不在字符串或注释内，检查代码块边界
        if (!state.inString && !state.inComment && !state.inBlockComment) {
            // 检查CHTL块
            if (isCHTLBlockStart(source, state.position)) {
                if (!currentFragment.empty()) {
                    // 保存当前片段
                    CodeFragment fragment;
                    fragment.type = state.currentType;
                    fragment.content = currentFragment;
                    fragment.startLine = fragmentLine;
                    fragment.startColumn = fragmentColumn;
                    fragment.endLine = state.line;
                    fragment.endColumn = state.column;
                    fragments.push_back(fragment);
                    currentFragment.clear();
                }
                state.inCHTLBlock = true;
                state.currentType = CodeFragmentType::CHTL;
                fragmentStart = state.position;
                fragmentLine = state.line;
                fragmentColumn = state.column;
            } else if (isCHTLBlockEnd(source, state.position)) {
                if (state.inCHTLBlock) {
                    state.inCHTLBlock = false;
                    // 保存CHTL片段
                    CodeFragment fragment;
                    fragment.type = CodeFragmentType::CHTL;
                    fragment.content = currentFragment;
                    fragment.startLine = fragmentLine;
                    fragment.startColumn = fragmentColumn;
                    fragment.endLine = state.line;
                    fragment.endColumn = state.column;
                    fragments.push_back(fragment);
                    currentFragment.clear();
                }
            }
            
            // 检查CHTL JS块
            if (isCHTLJSBlockStart(source, state.position)) {
                if (!currentFragment.empty()) {
                    // 保存当前片段
                    CodeFragment fragment;
                    fragment.type = state.currentType;
                    fragment.content = currentFragment;
                    fragment.startLine = fragmentLine;
                    fragment.startColumn = fragmentColumn;
                    fragment.endLine = state.line;
                    fragment.endColumn = state.column;
                    fragments.push_back(fragment);
                    currentFragment.clear();
                }
                state.inCHTLJSBlock = true;
                state.currentType = CodeFragmentType::CHTL_JS;
                fragmentStart = state.position;
                fragmentLine = state.line;
                fragmentColumn = state.column;
            } else if (isCHTLJSBlockEnd(source, state.position)) {
                if (state.inCHTLJSBlock) {
                    state.inCHTLJSBlock = false;
                    // 保存CHTL JS片段
                    CodeFragment fragment;
                    fragment.type = CodeFragmentType::CHTL_JS;
                    fragment.content = currentFragment;
                    fragment.startLine = fragmentLine;
                    fragment.startColumn = fragmentColumn;
                    fragment.endLine = state.line;
                    fragment.endColumn = state.column;
                    fragments.push_back(fragment);
                    currentFragment.clear();
                }
            }
            
            // 检查CSS块
            if (isCSSBlockStart(source, state.position)) {
                if (!currentFragment.empty()) {
                    // 保存当前片段
                    CodeFragment fragment;
                    fragment.type = state.currentType;
                    fragment.content = currentFragment;
                    fragment.startLine = fragmentLine;
                    fragment.startColumn = fragmentColumn;
                    fragment.endLine = state.line;
                    fragment.endColumn = state.column;
                    fragments.push_back(fragment);
                    currentFragment.clear();
                }
                state.inCSSBlock = true;
                state.currentType = CodeFragmentType::CSS;
                fragmentStart = state.position;
                fragmentLine = state.line;
                fragmentColumn = state.column;
            } else if (isCSSBlockEnd(source, state.position)) {
                if (state.inCSSBlock) {
                    state.inCSSBlock = false;
                    // 保存CSS片段
                    CodeFragment fragment;
                    fragment.type = CodeFragmentType::CSS;
                    fragment.content = currentFragment;
                    fragment.startLine = fragmentLine;
                    fragment.startColumn = fragmentColumn;
                    fragment.endLine = state.line;
                    fragment.endColumn = state.column;
                    fragments.push_back(fragment);
                    currentFragment.clear();
                }
            }
            
            // 检查JavaScript块
            if (isJavaScriptBlockStart(source, state.position)) {
                if (!currentFragment.empty()) {
                    // 保存当前片段
                    CodeFragment fragment;
                    fragment.type = state.currentType;
                    fragment.content = currentFragment;
                    fragment.startLine = fragmentLine;
                    fragment.startColumn = fragmentColumn;
                    fragment.endLine = state.line;
                    fragment.endColumn = state.column;
                    fragments.push_back(fragment);
                    currentFragment.clear();
                }
                state.inJavaScriptBlock = true;
                state.currentType = CodeFragmentType::JAVASCRIPT;
                fragmentStart = state.position;
                fragmentLine = state.line;
                fragmentColumn = state.column;
            } else if (isJavaScriptBlockEnd(source, state.position)) {
                if (state.inJavaScriptBlock) {
                    state.inJavaScriptBlock = false;
                    // 保存JavaScript片段
                    CodeFragment fragment;
                    fragment.type = CodeFragmentType::JAVASCRIPT;
                    fragment.content = currentFragment;
                    fragment.startLine = fragmentLine;
                    fragment.startColumn = fragmentColumn;
                    fragment.endLine = state.line;
                    fragment.endColumn = state.column;
                    fragments.push_back(fragment);
                    currentFragment.clear();
                }
            }
        }
        
        // 添加字符到当前片段
        currentFragment += c;
        state.position++;
    }
    
    // 保存最后一个片段
    if (!currentFragment.empty()) {
        CodeFragment fragment;
        fragment.type = state.currentType;
        fragment.content = currentFragment;
        fragment.startLine = fragmentLine;
        fragment.startColumn = fragmentColumn;
        fragment.endLine = state.line;
        fragment.endColumn = state.column;
        fragments.push_back(fragment);
    }
}

bool CHTLUnifiedScanner::isCHTLKeyword(const std::string& word) const {
    // TODO: 实现CHTL关键字识别
    return false;
}

bool CHTLUnifiedScanner::isCHTLBlockStart(const std::string& source, size_t pos) const {
    // 检查CHTL块开始标记
    if (pos + 5 < source.length()) {
        std::string substr = source.substr(pos, 6);
        if (substr == "[CHTL]") {
            return true;
        }
    }
    
    // 检查CHTL模板开始
    if (pos + 7 < source.length()) {
        std::string substr = source.substr(pos, 8);
        if (substr == "[Template]") {
            return true;
        }
    }
    
    // 检查CHTL自定义开始
    if (pos + 6 < source.length()) {
        std::string substr = source.substr(pos, 7);
        if (substr == "[Custom]") {
            return true;
        }
    }
    
    // 检查CHTL样式开始
    if (pos + 5 < source.length()) {
        std::string substr = source.substr(pos, 6);
        if (substr == "[Style]") {
            return true;
        }
    }
    
    // 检查CHTL脚本开始
    if (pos + 6 < source.length()) {
        std::string substr = source.substr(pos, 7);
        if (substr == "[Script]") {
            return true;
        }
    }
    
    // 检查CHTL导入开始
    if (pos + 6 < source.length()) {
        std::string substr = source.substr(pos, 7);
        if (substr == "[Import]") {
            return true;
        }
    }
    
    // 检查CHTL命名空间开始
    if (pos + 9 < source.length()) {
        std::string substr = source.substr(pos, 10);
        if (substr == "[Namespace]") {
            return true;
        }
    }
    
    // 检查CHTL配置开始
    if (pos + 12 < source.length()) {
        std::string substr = source.substr(pos, 13);
        if (substr == "[Configuration]") {
            return true;
        }
    }
    
    // 检查CHTL原始嵌入开始
    if (pos + 6 < source.length()) {
        std::string substr = source.substr(pos, 7);
        if (substr == "[Origin]") {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCHTLBlockEnd(const std::string& source, size_t pos) const {
    // 检查CHTL块结束标记
    if (pos + 5 < source.length()) {
        std::string substr = source.substr(pos, 6);
        if (substr == "[/CHTL]") {
            return true;
        }
    }
    
    // 检查CHTL模板结束
    if (pos + 8 < source.length()) {
        std::string substr = source.substr(pos, 9);
        if (substr == "[/Template]") {
            return true;
        }
    }
    
    // 检查CHTL自定义结束
    if (pos + 7 < source.length()) {
        std::string substr = source.substr(pos, 8);
        if (substr == "[/Custom]") {
            return true;
        }
    }
    
    // 检查CHTL样式结束
    if (pos + 6 < source.length()) {
        std::string substr = source.substr(pos, 7);
        if (substr == "[/Style]") {
            return true;
        }
    }
    
    // 检查CHTL脚本结束
    if (pos + 7 < source.length()) {
        std::string substr = source.substr(pos, 8);
        if (substr == "[/Script]") {
            return true;
        }
    }
    
    // 检查CHTL导入结束
    if (pos + 7 < source.length()) {
        std::string substr = source.substr(pos, 8);
        if (substr == "[/Import]") {
            return true;
        }
    }
    
    // 检查CHTL命名空间结束
    if (pos + 10 < source.length()) {
        std::string substr = source.substr(pos, 11);
        if (substr == "[/Namespace]") {
            return true;
        }
    }
    
    // 检查CHTL配置结束
    if (pos + 13 < source.length()) {
        std::string substr = source.substr(pos, 14);
        if (substr == "[/Configuration]") {
            return true;
        }
    }
    
    // 检查CHTL原始嵌入结束
    if (pos + 7 < source.length()) {
        std::string substr = source.substr(pos, 8);
        if (substr == "[/Origin]") {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCHTLJSKeyword(const std::string& word) const {
    // TODO: 实现CHTL JS关键字识别
    return false;
}

bool CHTLUnifiedScanner::isCHTLJSBlockStart(const std::string& source, size_t pos) const {
    // 检查CHTL JS块开始标记
    if (pos + 7 < source.length()) {
        std::string substr = source.substr(pos, 8);
        if (substr == "[CHTLJS]") {
            return true;
        }
    }
    
    // 检查CHTL JS脚本开始
    if (pos + 6 < source.length()) {
        std::string substr = source.substr(pos, 7);
        if (substr == "script {") {
            return true;
        }
    }
    
    // 检查CHTL JS监听器开始
    if (pos + 5 < source.length()) {
        std::string substr = source.substr(pos, 6);
        if (substr == "listen {") {
            return true;
        }
    }
    
    // 检查CHTL JS委托开始
    if (pos + 7 < source.length()) {
        std::string substr = source.substr(pos, 8);
        if (substr == "delegate {") {
            return true;
        }
    }
    
    // 检查CHTL JS动画开始
    if (pos + 7 < source.length()) {
        std::string substr = source.substr(pos, 8);
        if (substr == "animate {") {
            return true;
        }
    }
    
    // 检查CHTL JS虚拟对象开始
    if (pos + 2 < source.length()) {
        std::string substr = source.substr(pos, 3);
        if (substr == "vir") {
            return true;
        }
    }
    
    // 检查CHTL JS路由开始
    if (pos + 6 < source.length()) {
        std::string substr = source.substr(pos, 7);
        if (substr == "router {") {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCHTLJSBlockEnd(const std::string& source, size_t pos) const {
    // 检查CHTL JS块结束标记
    if (pos + 7 < source.length()) {
        std::string substr = source.substr(pos, 8);
        if (substr == "[/CHTLJS]") {
            return true;
        }
    }
    
    // 检查CHTL JS脚本结束
    if (pos + 1 < source.length()) {
        std::string substr = source.substr(pos, 2);
        if (substr == "}") {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCSSBlockStart(const std::string& source, size_t pos) const {
    // 检查CSS块开始标记
    if (pos + 4 < source.length()) {
        std::string substr = source.substr(pos, 5);
        if (substr == "[CSS]") {
            return true;
        }
    }
    
    // 检查CSS样式开始
    if (pos + 5 < source.length()) {
        std::string substr = source.substr(pos, 6);
        if (substr == "style {") {
            return true;
        }
    }
    
    // 检查CSS选择器开始
    if (pos + 1 < source.length()) {
        char c = source[pos];
        if (c == '.' || c == '#' || c == '@' || std::isalpha(c)) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCSSBlockEnd(const std::string& source, size_t pos) const {
    // 检查CSS块结束标记
    if (pos + 4 < source.length()) {
        std::string substr = source.substr(pos, 5);
        if (substr == "[/CSS]") {
            return true;
        }
    }
    
    // 检查CSS样式结束
    if (pos + 1 < source.length()) {
        std::string substr = source.substr(pos, 2);
        if (substr == "}") {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isJavaScriptBlockStart(const std::string& source, size_t pos) const {
    // 检查JavaScript块开始标记
    if (pos + 9 < source.length()) {
        std::string substr = source.substr(pos, 10);
        if (substr == "[JavaScript]") {
            return true;
        }
    }
    
    // 检查JavaScript脚本开始
    if (pos + 6 < source.length()) {
        std::string substr = source.substr(pos, 7);
        if (substr == "script {") {
            return true;
        }
    }
    
    // 检查JavaScript函数开始
    if (pos + 7 < source.length()) {
        std::string substr = source.substr(pos, 8);
        if (substr == "function") {
            return true;
        }
    }
    
    // 检查JavaScript变量开始
    if (pos + 2 < source.length()) {
        std::string substr = source.substr(pos, 3);
        if (substr == "var" || substr == "let" || substr == "const") {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isJavaScriptBlockEnd(const std::string& source, size_t pos) const {
    // 检查JavaScript块结束标记
    if (pos + 9 < source.length()) {
        std::string substr = source.substr(pos, 10);
        if (substr == "[/JavaScript]") {
            return true;
        }
    }
    
    // 检查JavaScript脚本结束
    if (pos + 1 < source.length()) {
        std::string substr = source.substr(pos, 2);
        if (substr == "}") {
            return true;
        }
    }
    
    return false;
}

std::string CHTLUnifiedScanner::replaceWithPlaceholders(const std::string& source, CodeFragmentType type) {
    std::string result = source;
    
    // 根据代码片段类型替换为相应的占位符
    switch (type) {
        case CodeFragmentType::CHTL:
            // 替换CHTL代码为占位符
            result = "_CHTL_CODE_PLACEHOLDER_";
            break;
        case CodeFragmentType::CHTL_JS:
            // 替换CHTL JS代码为占位符
            result = "_CHTLJS_CODE_PLACEHOLDER_";
            break;
        case CodeFragmentType::CSS:
            // 替换CSS代码为占位符
            result = "_CSS_CODE_PLACEHOLDER_";
            break;
        case CodeFragmentType::JAVASCRIPT:
            // 替换JavaScript代码为占位符
            result = "_JS_CODE_PLACEHOLDER_";
            break;
        default:
            // 保持原样
            break;
    }
    
    return result;
}

std::string CHTLUnifiedScanner::restoreFromPlaceholders(const std::string& source) {
    std::string result = source;
    
    // 恢复CHTL代码占位符
    size_t pos = result.find("_CHTL_CODE_PLACEHOLDER_");
    while (pos != std::string::npos) {
        result.replace(pos, 23, "CHTL代码");
        pos = result.find("_CHTL_CODE_PLACEHOLDER_", pos + 7);
    }
    
    // 恢复CHTL JS代码占位符
    pos = result.find("_CHTLJS_CODE_PLACEHOLDER_");
    while (pos != std::string::npos) {
        result.replace(pos, 25, "CHTLJS代码");
        pos = result.find("_CHTLJS_CODE_PLACEHOLDER_", pos + 9);
    }
    
    // 恢复CSS代码占位符
    pos = result.find("_CSS_CODE_PLACEHOLDER_");
    while (pos != std::string::npos) {
        result.replace(pos, 22, "CSS代码");
        pos = result.find("_CSS_CODE_PLACEHOLDER_", pos + 6);
    }
    
    // 恢复JavaScript代码占位符
    pos = result.find("_JS_CODE_PLACEHOLDER_");
    while (pos != std::string::npos) {
        result.replace(pos, 21, "JavaScript代码");
        pos = result.find("_JS_CODE_PLACEHOLDER_", pos + 12);
    }
    
    return result;
}

bool CHTLUnifiedScanner::isInString(const std::string& source, size_t pos) const {
    bool inString = false;
    char stringChar = 0;
    
    for (size_t i = 0; i < pos && i < source.length(); ++i) {
        char c = source[i];
        
        if (c == '"' || c == '\'') {
            if (!inString) {
                inString = true;
                stringChar = c;
            } else if (c == stringChar) {
                inString = false;
            }
        }
    }
    
    return inString;
}

bool CHTLUnifiedScanner::isInComment(const std::string& source, size_t pos) const {
    bool inComment = false;
    bool inBlockComment = false;
    
    for (size_t i = 0; i < pos && i < source.length(); ++i) {
        char c = source[i];
        
        if (c == '/' && i + 1 < source.length() && source[i + 1] == '/') {
            inComment = true;
        } else if (c == '\n' && inComment) {
            inComment = false;
        } else if (c == '/' && i + 1 < source.length() && source[i + 1] == '*') {
            inBlockComment = true;
        } else if (c == '*' && i + 1 < source.length() && source[i + 1] == '/' && inBlockComment) {
            inBlockComment = false;
        }
    }
    
    return inComment || inBlockComment;
}

bool CHTLUnifiedScanner::isInBlockComment(const std::string& source, size_t pos) const {
    bool inBlockComment = false;
    
    for (size_t i = 0; i < pos && i < source.length(); ++i) {
        char c = source[i];
        
        if (c == '/' && i + 1 < source.length() && source[i + 1] == '*') {
            inBlockComment = true;
        } else if (c == '*' && i + 1 < source.length() && source[i + 1] == '/' && inBlockComment) {
            inBlockComment = false;
        }
    }
    
    return inBlockComment;
}

bool CHTLUnifiedScanner::isInLineComment(const std::string& source, size_t pos) const {
    bool inComment = false;
    
    for (size_t i = 0; i < pos && i < source.length(); ++i) {
        char c = source[i];
        
        if (c == '/' && i + 1 < source.length() && source[i + 1] == '/') {
            inComment = true;
        } else if (c == '\n' && inComment) {
            inComment = false;
        }
    }
    
    return inComment;
}

bool CHTLUnifiedScanner::isBalanced(const std::string& source, size_t start, size_t end) const {
    std::stack<char> brackets;
    
    for (size_t i = start; i < end && i < source.length(); ++i) {
        char c = source[i];
        
        if (c == '(' || c == '[' || c == '{') {
            brackets.push(c);
        } else if (c == ')' || c == ']' || c == '}') {
            if (brackets.empty()) {
                return false;
            }
            
            char top = brackets.top();
            brackets.pop();
            
            if ((c == ')' && top != '(') ||
                (c == ']' && top != '[') ||
                (c == '}' && top != '{')) {
                return false;
            }
        }
    }
    
    return brackets.empty();
}

int CHTLUnifiedScanner::findMatchingBrace(const std::string& source, size_t pos) const {
    if (pos >= source.length() || source[pos] != '{') {
        return -1;
    }
    
    int depth = 1;
    size_t i = pos + 1;
    
    while (i < source.length() && depth > 0) {
        char c = source[i];
        
        if (c == '{') {
            depth++;
        } else if (c == '}') {
            depth--;
        }
        
        i++;
    }
    
    return (depth == 0) ? static_cast<int>(i - 1) : -1;
}

int CHTLUnifiedScanner::findMatchingBracket(const std::string& source, size_t pos) const {
    if (pos >= source.length() || source[pos] != '[') {
        return -1;
    }
    
    int depth = 1;
    size_t i = pos + 1;
    
    while (i < source.length() && depth > 0) {
        char c = source[i];
        
        if (c == '[') {
            depth++;
        } else if (c == ']') {
            depth--;
        }
        
        i++;
    }
    
    return (depth == 0) ? static_cast<int>(i - 1) : -1;
}

int CHTLUnifiedScanner::findMatchingParen(const std::string& source, size_t pos) const {
    if (pos >= source.length() || source[pos] != '(') {
        return -1;
    }
    
    int depth = 1;
    size_t i = pos + 1;
    
    while (i < source.length() && depth > 0) {
        char c = source[i];
        
        if (c == '(') {
            depth++;
        } else if (c == ')') {
            depth--;
        }
        
        i++;
    }
    
    return (depth == 0) ? static_cast<int>(i - 1) : -1;
}

bool CHTLUnifiedScanner::isCodeBoundary(const std::string& source, size_t pos) const {
    if (pos >= source.length()) {
        return false;
    }
    
    char c = source[pos];
    
    // 检查是否是代码块标记
    if (c == '[') {
        // 检查是否是CHTL块标记
        if (pos + 5 < source.length()) {
            std::string substr = source.substr(pos, 6);
            if (substr == "[CHTL]" || substr == "[CSS]" || substr == "[JavaScript]") {
                return true;
            }
        }
        
        // 检查是否是CHTL JS块标记
        if (pos + 7 < source.length()) {
            std::string substr = source.substr(pos, 8);
            if (substr == "[CHTLJS]") {
                return true;
            }
        }
        
        // 检查是否是其他块标记
        if (pos + 6 < source.length()) {
            std::string substr = source.substr(pos, 7);
            if (substr == "[Style]" || substr == "[Script]" || substr == "[Import]" || 
                substr == "[Origin]" || substr == "[Custom]" || substr == "[Template]") {
                return true;
            }
        }
    }
    
    // 检查是否是代码块结束标记
    if (c == ']' && pos > 0 && source[pos - 1] == '/') {
        return true;
    }
    
    // 检查是否是代码块开始标记
    if (c == '{' && pos > 0) {
        char prev = source[pos - 1];
        if (prev == ' ' || prev == '\t' || prev == '\n' || prev == '\r') {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isFragmentBoundary(const std::string& source, size_t pos) const {
    if (pos >= source.length()) {
        return false;
    }
    
    char c = source[pos];
    
    // 检查是否是代码块标记
    if (c == '[') {
        return true;
    }
    
    // 检查是否是代码块结束标记
    if (c == ']' && pos > 0 && source[pos - 1] == '/') {
        return true;
    }
    
    // 检查是否是代码块开始标记
    if (c == '{' && pos > 0) {
        char prev = source[pos - 1];
        if (prev == ' ' || prev == '\t' || prev == '\n' || prev == '\r') {
            return true;
        }
    }
    
    // 检查是否是代码块结束标记
    if (c == '}' && pos > 0) {
        char prev = source[pos - 1];
        if (prev == ' ' || prev == '\t' || prev == '\n' || prev == '\r') {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isWideMode(const std::string& source, size_t pos) const {
    // 宽判模式：允许更多的代码片段类型
    // 检查是否是宽判模式标记
    if (pos + 8 < source.length()) {
        std::string substr = source.substr(pos, 9);
        if (substr == "[WideMode]") {
            return true;
        }
    }
    
    // 检查是否是宽判模式配置
    if (pos + 12 < source.length()) {
        std::string substr = source.substr(pos, 13);
        if (substr == "[Configuration]") {
            return true;
        }
    }
    
    // 默认启用宽判模式
    return true;
}

bool CHTLUnifiedScanner::isStrictMode(const std::string& source, size_t pos) const {
    // 严判模式：只允许特定的代码片段类型
    // 检查是否是严判模式标记
    if (pos + 9 < source.length()) {
        std::string substr = source.substr(pos, 10);
        if (substr == "[StrictMode]") {
            return true;
        }
    }
    
    // 检查是否是严判模式配置
    if (pos + 12 < source.length()) {
        std::string substr = source.substr(pos, 13);
        if (substr == "[Configuration]") {
            return true;
        }
    }
    
    // 默认不启用严判模式
    return false;
}

char CHTLUnifiedScanner::getChar(const std::string& source, size_t pos) const {
    return (pos < source.length()) ? source[pos] : '\0';
}

std::string CHTLUnifiedScanner::getWord(const std::string& source, size_t pos) const {
    if (pos >= source.length()) {
        return "";
    }
    
    std::string word;
    size_t i = pos;
    
    // 跳过空白字符
    while (i < source.length() && isWhitespace(source[i])) {
        i++;
    }
    
    // 提取单词
    while (i < source.length() && isAlphaNumeric(source[i])) {
        word += source[i];
        i++;
    }
    
    return word;
}

bool CHTLUnifiedScanner::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool CHTLUnifiedScanner::isAlphaNumeric(char c) const {
    return std::isalnum(c) || c == '_' || c == '-';
}

bool CHTLUnifiedScanner::isOperator(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '!' || c == '<' || c == '>' || c == '&' || c == '|' ||
           c == ':' || c == ';' || c == ',' || c == '.' || c == '?' || c == '~';
}

void CHTLUnifiedScanner::addError(const std::string& error) {
    m_errors.push_back(error);
}

void CHTLUnifiedScanner::addWarning(const std::string& warning) {
    m_warnings.push_back(warning);
}

void CHTLUnifiedScanner::addInfo(const std::string& info) {
    m_info.push_back(info);
}

const std::vector<std::string>& CHTLUnifiedScanner::getErrors() const {
    return m_errors;
}

const std::vector<std::string>& CHTLUnifiedScanner::getWarnings() const {
    return m_warnings;
}

const std::vector<std::string>& CHTLUnifiedScanner::getInfo() const {
    return m_info;
}

void CHTLUnifiedScanner::clearErrors() {
    m_errors.clear();
}

void CHTLUnifiedScanner::clearWarnings() {
    m_warnings.clear();
}

void CHTLUnifiedScanner::clearInfo() {
    m_info.clear();
}

void CHTLUnifiedScanner::clearAll() {
    clearErrors();
    clearWarnings();
    clearInfo();
}

bool CHTLUnifiedScanner::hasErrors() const {
    return !m_errors.empty();
}

bool CHTLUnifiedScanner::hasWarnings() const {
    return !m_warnings.empty();
}

bool CHTLUnifiedScanner::hasInfo() const {
    return !m_info.empty();
}

bool CHTLUnifiedScanner::hasAny() const {
    return hasErrors() || hasWarnings() || hasInfo();
}

} // namespace CHTL