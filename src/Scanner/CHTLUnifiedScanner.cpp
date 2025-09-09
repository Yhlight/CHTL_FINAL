#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner()
    : m_placeholder("_JS_CODE_PLACEHOLDER_")
    , m_scanMode(ScanMode::WIDE_JUDGMENT)
    , m_debugMode(false)
    , m_currentPos(0)
    , m_currentLine(1)
    , m_currentColumn(1)
    , m_inString(false)
    , m_inComment(false)
    , m_inCHTLBlock(false)
    , m_inCHTLJSBlock(false)
    , m_inStyleBlock(false)
    , m_inScriptBlock(false)
    , m_braceCount(0)
    , m_parenCount(0)
    , m_bracketCount(0)
{
}

CHTLUnifiedScanner::~CHTLUnifiedScanner() = default;

std::vector<CodeFragment> CHTLUnifiedScanner::scan(const std::string& sourceCode) {
    m_sourceCode = sourceCode;
    resetState();
    
    std::vector<CodeFragment> fragments;
    
    debugOutput("开始扫描源代码，长度: " + std::to_string(sourceCode.length()));
    
    while (m_currentPos < m_sourceCode.length()) {
        skipWhitespace();
        
        if (m_currentPos >= m_sourceCode.length()) {
            break;
        }
        
        // 根据当前状态和语法特征判断代码类型
        if (isCHTLSyntax()) {
            debugOutput("检测到 CHTL 语法，位置: " + std::to_string(m_currentPos));
            fragments.push_back(scanCHTLBlock());
        } else if (isCHTLJSSyntax()) {
            debugOutput("检测到 CHTL JS 语法，位置: " + std::to_string(m_currentPos));
            fragments.push_back(scanCHTLJSBlock());
        } else if (isCSSSyntax()) {
            debugOutput("检测到 CSS 语法，位置: " + std::to_string(m_currentPos));
            fragments.push_back(scanCSSBlock());
        } else if (isJSSyntax()) {
            debugOutput("检测到 JS 语法，位置: " + std::to_string(m_currentPos));
            fragments.push_back(scanJSBlock());
        } else {
            // 默认为 CHTL 语法
            debugOutput("默认为 CHTL 语法，位置: " + std::to_string(m_currentPos));
            fragments.push_back(scanCHTLBlock());
        }
    }
    
    debugOutput("扫描完成，共生成 " + std::to_string(fragments.size()) + " 个代码片段");
    return fragments;
}

void CHTLUnifiedScanner::setPlaceholder(const std::string& placeholder) {
    m_placeholder = placeholder;
}

void CHTLUnifiedScanner::setScanMode(ScanMode mode) {
    m_scanMode = mode;
}

void CHTLUnifiedScanner::setDebugMode(bool enabled) {
    m_debugMode = enabled;
}

void CHTLUnifiedScanner::resetState() {
    m_currentPos = 0;
    m_currentLine = 1;
    m_currentColumn = 1;
    m_inString = false;
    m_inComment = false;
    m_inCHTLBlock = false;
    m_inCHTLJSBlock = false;
    m_inStyleBlock = false;
    m_inScriptBlock = false;
    m_braceCount = 0;
    m_parenCount = 0;
    m_bracketCount = 0;
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (m_currentPos < m_sourceCode.length()) {
        char c = m_sourceCode[m_currentPos];
        
        if (c == '\n') {
            m_currentLine++;
            m_currentColumn = 1;
        } else if (c == '\t') {
            m_currentColumn += 4; // 假设制表符宽度为4
        } else if (std::isspace(c)) {
            m_currentColumn++;
        } else {
            break;
        }
        
        m_currentPos++;
    }
}

bool CHTLUnifiedScanner::skipComment() {
    if (m_currentPos >= m_sourceCode.length()) {
        return false;
    }
    
    // 检查单行注释 //
    if (m_currentPos + 1 < m_sourceCode.length() && 
        m_sourceCode[m_currentPos] == '/' && 
        m_sourceCode[m_currentPos + 1] == '/') {
        
        // 跳过到行尾
        while (m_currentPos < m_sourceCode.length() && m_sourceCode[m_currentPos] != '\n') {
            m_currentPos++;
        }
        return true;
    }
    
    // 检查多行注释 /* */
    if (m_currentPos + 1 < m_sourceCode.length() && 
        m_sourceCode[m_currentPos] == '/' && 
        m_sourceCode[m_currentPos + 1] == '*') {
        
        m_currentPos += 2;
        while (m_currentPos + 1 < m_sourceCode.length()) {
            if (m_sourceCode[m_currentPos] == '*' && m_sourceCode[m_currentPos + 1] == '/') {
                m_currentPos += 2;
                return true;
            }
            m_currentPos++;
        }
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::skipString() {
    if (m_currentPos >= m_sourceCode.length()) {
        return false;
    }
    
    char quote = m_sourceCode[m_currentPos];
    if (quote != '"' && quote != '\'') {
        return false;
    }
    
    m_currentPos++; // 跳过开始引号
    
    while (m_currentPos < m_sourceCode.length()) {
        if (m_sourceCode[m_currentPos] == quote) {
            m_currentPos++; // 跳过结束引号
            return true;
        }
        
        // 处理转义字符
        if (m_sourceCode[m_currentPos] == '\\' && m_currentPos + 1 < m_sourceCode.length()) {
            m_currentPos += 2;
        } else {
            m_currentPos++;
        }
    }
    
    return true; // 即使没有找到结束引号也返回true
}

bool CHTLUnifiedScanner::isCHTLSyntax() {
    // 检查 CHTL 特有的语法特征
    if (m_currentPos >= m_sourceCode.length()) {
        return false;
    }
    
    // 检查模板语法 [Template], [Custom], [Import] 等
    if (m_sourceCode[m_currentPos] == '[') {
        size_t endPos = m_sourceCode.find(']', m_currentPos);
        if (endPos != std::string::npos) {
            std::string keyword = m_sourceCode.substr(m_currentPos + 1, endPos - m_currentPos - 1);
            if (keyword == "Template" || keyword == "Custom" || keyword == "Import" || 
                keyword == "Origin" || keyword == "Namespace" || keyword == "Configuration") {
                return true;
            }
        }
    }
    
    // 检查 HTML 元素语法
    if (std::isalpha(m_sourceCode[m_currentPos])) {
        // 简单的 HTML 元素检测
        size_t endPos = m_currentPos;
        while (endPos < m_sourceCode.length() && 
               (std::isalnum(m_sourceCode[endPos]) || m_sourceCode[endPos] == '-')) {
            endPos++;
        }
        
        if (endPos < m_sourceCode.length() && m_sourceCode[endPos] == '{') {
            return true;
        }
    }
    
    // 检查 text 语法
    if (m_currentPos + 4 < m_sourceCode.length() && 
        m_sourceCode.substr(m_currentPos, 4) == "text") {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCHTLJSSyntax() {
    // 检查 CHTL JS 特有的语法特征
    if (m_currentPos >= m_sourceCode.length()) {
        return false;
    }
    
    // 检查双大括号语法 {{ }}
    if (m_currentPos + 1 < m_sourceCode.length() && 
        m_sourceCode[m_currentPos] == '{' && 
        m_sourceCode[m_currentPos + 1] == '{') {
        return true;
    }
    
    // 检查 CHTL JS 函数语法
    std::vector<std::string> chtmlJSKeywords = {
        "fileloader", "listen", "delegate", "animate", "vir", "router", "printMylove", "iNeverAway"
    };
    
    for (const auto& keyword : chtmlJSKeywords) {
        if (m_currentPos + keyword.length() < m_sourceCode.length() && 
            m_sourceCode.substr(m_currentPos, keyword.length()) == keyword) {
            // 检查后面是否跟着 {
            size_t nextPos = m_currentPos + keyword.length();
            skipWhitespace();
            if (nextPos < m_sourceCode.length() && m_sourceCode[nextPos] == '{') {
                return true;
            }
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCSSSyntax() {
    // 检查 CSS 语法特征
    if (m_currentPos >= m_sourceCode.length()) {
        return false;
    }
    
    // 检查 CSS 选择器语法
    if (m_sourceCode[m_currentPos] == '.' || m_sourceCode[m_currentPos] == '#' || 
        m_sourceCode[m_currentPos] == '@') {
        return true;
    }
    
    // 检查 CSS 属性语法
    if (std::isalpha(m_sourceCode[m_currentPos])) {
        size_t endPos = m_currentPos;
        while (endPos < m_sourceCode.length() && 
               (std::isalnum(m_sourceCode[endPos]) || m_sourceCode[endPos] == '-')) {
            endPos++;
        }
        
        if (endPos < m_sourceCode.length() && m_sourceCode[endPos] == ':') {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isJSSyntax() {
    // 检查 JavaScript 语法特征
    if (m_currentPos >= m_sourceCode.length()) {
        return false;
    }
    
    // 检查 JavaScript 关键字
    std::vector<std::string> jsKeywords = {
        "function", "var", "let", "const", "if", "else", "for", "while", 
        "return", "class", "import", "export", "async", "await"
    };
    
    for (const auto& keyword : jsKeywords) {
        if (m_currentPos + keyword.length() < m_sourceCode.length() && 
            m_sourceCode.substr(m_currentPos, keyword.length()) == keyword) {
            // 检查是否为完整的单词
            size_t nextPos = m_currentPos + keyword.length();
            if (nextPos >= m_sourceCode.length() || !std::isalnum(m_sourceCode[nextPos])) {
                return true;
            }
        }
    }
    
    return false;
}

CodeFragment CHTLUnifiedScanner::scanCHTLBlock() {
    size_t startPos = m_currentPos;
    int startLine = m_currentLine;
    int startColumn = m_currentColumn;
    
    debugOutput("开始扫描 CHTL 块，位置: " + std::to_string(startPos));
    
    while (m_currentPos < m_sourceCode.length()) {
        // 跳过注释和字符串
        if (skipComment() || skipString()) {
            continue;
        }
        
        char c = m_sourceCode[m_currentPos];
        
        // 检查是否遇到其他类型的代码
        if (isCHTLJSSyntax() || isCSSSyntax() || isJSSyntax()) {
            break;
        }
        
        // 处理括号计数
        if (c == '{') {
            m_braceCount++;
        } else if (c == '}') {
            m_braceCount--;
            if (m_braceCount == 0) {
                m_currentPos++;
                break;
            }
        }
        
        m_currentPos++;
    }
    
    std::string content = m_sourceCode.substr(startPos, m_currentPos - startPos);
    debugOutput("CHTL 块内容长度: " + std::to_string(content.length()));
    
    return CodeFragment(CodeFragmentType::CHTL, content, startPos, m_currentPos, startLine, startColumn);
}

CodeFragment CHTLUnifiedScanner::scanCHTLJSBlock() {
    size_t startPos = m_currentPos;
    int startLine = m_currentLine;
    int startColumn = m_currentColumn;
    
    debugOutput("开始扫描 CHTL JS 块，位置: " + std::to_string(startPos));
    
    // 使用占位符机制分离 CHTL JS 和 JS 代码
    std::string content = applyPlaceholderMechanism(m_sourceCode.substr(startPos));
    
    // 找到 CHTL JS 代码的结束位置
    size_t endPos = startPos;
    while (endPos < m_sourceCode.length()) {
        if (isCHTLSyntax() || isCSSSyntax() || isJSSyntax()) {
            break;
        }
        endPos++;
    }
    
    content = m_sourceCode.substr(startPos, endPos - startPos);
    m_currentPos = endPos;
    
    debugOutput("CHTL JS 块内容长度: " + std::to_string(content.length()));
    
    return CodeFragment(CodeFragmentType::CHTL_JS, content, startPos, endPos, startLine, startColumn);
}

CodeFragment CHTLUnifiedScanner::scanCSSBlock() {
    size_t startPos = m_currentPos;
    int startLine = m_currentLine;
    int startColumn = m_currentColumn;
    
    debugOutput("开始扫描 CSS 块，位置: " + std::to_string(startPos));
    
    while (m_currentPos < m_sourceCode.length()) {
        char c = m_sourceCode[m_currentPos];
        
        // 检查是否遇到其他类型的代码
        if (isCHTLSyntax() || isCHTLJSSyntax() || isJSSyntax()) {
            break;
        }
        
        // 处理 CSS 块结束
        if (c == '}' && m_braceCount == 0) {
            m_currentPos++;
            break;
        }
        
        if (c == '{') {
            m_braceCount++;
        } else if (c == '}') {
            m_braceCount--;
        }
        
        m_currentPos++;
    }
    
    std::string content = m_sourceCode.substr(startPos, m_currentPos - startPos);
    debugOutput("CSS 块内容长度: " + std::to_string(content.length()));
    
    return CodeFragment(CodeFragmentType::CSS, content, startPos, m_currentPos, startLine, startColumn);
}

CodeFragment CHTLUnifiedScanner::scanJSBlock() {
    size_t startPos = m_currentPos;
    int startLine = m_currentLine;
    int startColumn = m_currentColumn;
    
    debugOutput("开始扫描 JS 块，位置: " + std::to_string(startPos));
    
    while (m_currentPos < m_sourceCode.length()) {
        char c = m_sourceCode[m_currentPos];
        
        // 检查是否遇到其他类型的代码
        if (isCHTLSyntax() || isCHTLJSSyntax() || isCSSSyntax()) {
            break;
        }
        
        // 处理 JavaScript 块结束
        if (c == '}' && m_braceCount == 0) {
            m_currentPos++;
            break;
        }
        
        if (c == '{') {
            m_braceCount++;
        } else if (c == '}') {
            m_braceCount--;
        }
        
        m_currentPos++;
    }
    
    std::string content = m_sourceCode.substr(startPos, m_currentPos - startPos);
    debugOutput("JS 块内容长度: " + std::to_string(content.length()));
    
    return CodeFragment(CodeFragmentType::JS, content, startPos, m_currentPos, startLine, startColumn);
}

std::string CHTLUnifiedScanner::applyPlaceholderMechanism(const std::string& content) {
    std::string result = content;
    
    // 使用占位符替换 JS 代码，保留 CHTL JS 语法
    // 这里需要实现复杂的占位符机制
    // 暂时返回原内容，后续需要完善
    
    debugOutput("应用占位符机制，原始长度: " + std::to_string(content.length()));
    
    return result;
}

std::vector<std::string> CHTLUnifiedScanner::doublePointerScan(const std::string& content, const std::string& keyword) {
    std::vector<std::string> result;
    
    size_t front = 0;
    size_t back = 0;
    
    while (front < content.length()) {
        // 预先扫描是否存在关键字
        size_t keywordPos = content.find(keyword, front);
        if (keywordPos == std::string::npos) {
            // 没有找到关键字，移动前指针
            front = content.length();
            break;
        }
        
        // 找到关键字，通知后指针准备收集
        if (back < keywordPos) {
            result.push_back(content.substr(back, keywordPos - back));
        }
        
        result.push_back(keyword);
        front = keywordPos + keyword.length();
        back = front;
    }
    
    // 收集剩余内容
    if (back < content.length()) {
        result.push_back(content.substr(back));
    }
    
    return result;
}

std::string CHTLUnifiedScanner::preprocessContent(const std::string& content) {
    // 前置截取处理
    // 暂时返回原内容，后续需要完善
    return content;
}

size_t CHTLUnifiedScanner::smartExpandRange(size_t startPos, size_t endPos) {
    // 智能扩增扫描范围
    // 暂时返回原结束位置，后续需要完善
    return endPos;
}

bool CHTLUnifiedScanner::isSyntaxBoundary(size_t pos) {
    if (pos >= m_sourceCode.length()) {
        return true;
    }
    
    char c = m_sourceCode[pos];
    return c == '{' || c == '}' || c == ';' || c == '\n';
}

char CHTLUnifiedScanner::peekNext() {
    if (m_currentPos + 1 < m_sourceCode.length()) {
        return m_sourceCode[m_currentPos + 1];
    }
    return '\0';
}

char CHTLUnifiedScanner::peekAt(size_t offset) {
    if (m_currentPos + offset < m_sourceCode.length()) {
        return m_sourceCode[m_currentPos + offset];
    }
    return '\0';
}

void CHTLUnifiedScanner::advance(size_t steps) {
    m_currentPos += steps;
    m_currentColumn += steps;
}

void CHTLUnifiedScanner::debugOutput(const std::string& message) {
    if (m_debugMode) {
        std::cout << "[CHTLUnifiedScanner] " << message << std::endl;
    }
}

bool CHTLUnifiedScanner::isBlockStart(size_t pos) {
    if (pos >= m_sourceCode.length()) {
        return false;
    }
    return m_sourceCode[pos] == '{';
}

bool CHTLUnifiedScanner::isBlockEnd(size_t pos) {
    if (pos >= m_sourceCode.length()) {
        return false;
    }
    return m_sourceCode[pos] == '}';
}

size_t CHTLUnifiedScanner::findMatchingEnd(size_t startPos, char startChar, char endChar) {
    int count = 0;
    for (size_t i = startPos; i < m_sourceCode.length(); i++) {
        if (m_sourceCode[i] == startChar) {
            count++;
        } else if (m_sourceCode[i] == endChar) {
            count--;
            if (count == 0) {
                return i;
            }
        }
    }
    return std::string::npos;
}

size_t CHTLUnifiedScanner::handleNestedStructure(size_t startPos) {
    // 处理嵌套结构
    // 暂时返回原位置，后续需要完善
    return startPos;
}

} // namespace CHTL