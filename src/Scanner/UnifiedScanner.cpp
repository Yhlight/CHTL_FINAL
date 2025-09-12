#include "CHTL/Scanner/UnifiedScanner.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL {

UnifiedScanner::UnifiedScanner(std::shared_ptr<CHTLContext> context)
    : m_placeholderPrefix("_PLACEHOLDER_")
    , m_debugMode(false)
{
    // 初始化CHTL关键字
    m_chtlKeywords = {
        {"text", true}, {"style", true}, {"script", true},
        {"[Template]", true}, {"[Custom]", true}, {"[Origin]", true},
        {"[Import]", true}, {"[Namespace]", true}, {"[Configuration]", true},
        {"[Info]", true}, {"[Export]", true}, {"use", true},
        {"inherit", true}, {"delete", true}, {"insert", true},
        {"after", true}, {"before", true}, {"replace", true},
        {"at", true}, {"top", true}, {"bottom", true},
        {"from", true}, {"as", true}, {"except", true},
        {"html5", true}
    };
    
    // 初始化CHTL JS关键字
    m_chtlJSKeywords = {
        {"vir", true}, {"listen", true}, {"animate", true},
        {"router", true}, {"delegate", true}, {"fileloader", true},
        {"util", true}, {"change", true}, {"then", true}
    };
    
    // 初始化占位符计数器
    m_placeholderCounters[CodeFragmentType::CHTL] = 0;
    m_placeholderCounters[CodeFragmentType::CHTL_JS] = 0;
    m_placeholderCounters[CodeFragmentType::CSS] = 0;
    m_placeholderCounters[CodeFragmentType::JS] = 0;
    m_placeholderCounters[CodeFragmentType::HTML] = 0;
}

UnifiedScanner::~UnifiedScanner() {
}

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& sourceCode) {
    std::vector<CodeFragment> fragments;
    
    if (sourceCode.empty()) {
        return fragments;
    }
    
    // 预扫描，识别语法边界
    auto boundaries = preScan(sourceCode);
    
    if (boundaries.empty()) {
        // 没有识别到特殊语法，整个代码作为HTML处理
        fragments.emplace_back(CodeFragmentType::HTML, sourceCode, 1, 1);
        return fragments;
    }
    
        // 根据边界信息分离代码片段
        size_t lastPos = 0;
        for (const auto& boundary : boundaries) {
            size_t startPos = boundary.first;
            CodeFragmentType type = boundary.second;
            
            std::cout << "[UnifiedScanner] Processing boundary: startPos=" << startPos 
                      << ", type=" << static_cast<int>(type) << std::endl;
        
        // 添加边界前的代码（如果有）
        if (startPos > lastPos) {
            std::string content = sourceCode.substr(lastPos, startPos - lastPos);
            if (!content.empty() && !std::all_of(content.begin(), content.end(), ::isspace)) {
                auto [startLine, startCol] = getLineColumn(sourceCode, lastPos);
                auto [endLine, endCol] = getLineColumn(sourceCode, startPos - 1);
                fragments.emplace_back(CodeFragmentType::HTML, content, startLine, startCol);
            }
        }
        
        // 识别并添加当前类型的代码片段
        int endPos = -1;
        switch (type) {
            case CodeFragmentType::CHTL:
                endPos = identifyCHTLBoundary(sourceCode, startPos);
                std::cout << "[UnifiedScanner] CHTL boundary: startPos=" << startPos << ", endPos=" << endPos << std::endl;
                break;
            case CodeFragmentType::CHTL_JS:
                endPos = identifyCHTLJSBoundary(sourceCode, startPos);
                break;
            case CodeFragmentType::CSS:
                endPos = identifyCSSBoundary(sourceCode, startPos);
                break;
            case CodeFragmentType::JS:
                endPos = identifyJSBoundary(sourceCode, startPos);
                break;
            case CodeFragmentType::HTML:
                endPos = identifyHTMLBoundary(sourceCode, startPos);
                break;
            default:
                break;
        }
        
        if (endPos > 0) {
            std::string content = sourceCode.substr(startPos, endPos - startPos);
            auto [startLine, startCol] = getLineColumn(sourceCode, startPos);
            auto [endLine, endCol] = getLineColumn(sourceCode, endPos - 1);
            
            CodeFragment fragment(type, content, startLine, startCol);
            // placeholder functionality removed for compatibility
            std::cout << "[UnifiedScanner] Creating fragment: type=" << static_cast<int>(type) 
                      << ", content=\"" << content.substr(0, 50) << "...\", line=" << startLine << std::endl;
            fragments.push_back(fragment);
            
            lastPos = endPos;
        } else {
            lastPos = startPos + 1;
        }
    }
    
    // 添加剩余的代码
    if (lastPos < sourceCode.length()) {
        std::string content = sourceCode.substr(lastPos);
        if (!content.empty() && !std::all_of(content.begin(), content.end(), ::isspace)) {
            auto [startLine, startCol] = getLineColumn(sourceCode, lastPos);
            auto [endLine, endCol] = getLineColumn(sourceCode, sourceCode.length() - 1);
            fragments.emplace_back(CodeFragmentType::HTML, content, startLine, startCol);
        }
    }
    
    if (m_debugMode) {
        std::cout << "[UnifiedScanner] Found " << fragments.size() << " code fragments:" << std::endl;
        for (const auto& fragment : fragments) {
            std::cout << "  Type: " << static_cast<int>(fragment.type) 
                      << ", Lines: " << fragment.line << "-" << fragment.line
                      << ", Column: " << fragment.column << std::endl;
        }
    }
    
    return fragments;
}

void UnifiedScanner::setPlaceholderPrefix(const std::string& prefix) {
    m_placeholderPrefix = prefix;
}

const std::string& UnifiedScanner::getPlaceholderPrefix() const {
    return m_placeholderPrefix;
}

void UnifiedScanner::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool UnifiedScanner::isDebugMode() const {
    return m_debugMode;
}

std::vector<std::pair<size_t, CodeFragmentType>> UnifiedScanner::preScan(const std::string& sourceCode) {
    std::vector<std::pair<size_t, CodeFragmentType>> boundaries;
    
    size_t pos = 0;
    while (pos < sourceCode.length()) {
        pos = skipWhitespace(sourceCode, pos);
        if (pos >= sourceCode.length()) break;
        
        // 检查注释
        if (pos + 1 < sourceCode.length() && 
            sourceCode[pos] == '/' && sourceCode[pos + 1] == '/') {
            pos = skipComment(sourceCode, pos);
            continue;
        }
        
        // 检查CHTL关键字
        if (std::isalpha(sourceCode[pos])) {
            size_t startPos = pos;
            while (pos < sourceCode.length() && (std::isalnum(sourceCode[pos]) || sourceCode[pos] == '-')) {
                pos++;
            }
            
            std::string identifier = sourceCode.substr(startPos, pos - startPos);
            
            // 检查是否为CHTL关键字
            if (m_chtlKeywords.find(identifier) != m_chtlKeywords.end()) {
                // 跳过空格
                size_t checkPos = pos;
                while (checkPos < sourceCode.length() && std::isspace(sourceCode[checkPos])) {
                    checkPos++;
                }
                
                // 检查后面是否有 {
                if (checkPos < sourceCode.length() && sourceCode[checkPos] == '{') {
                    boundaries.emplace_back(startPos, CodeFragmentType::CHTL);
                    pos = checkPos;
                    continue;
                }
            }
            
            // 检查是否为HTML元素（只有在不是CHTL关键字的情况下）
            // 跳过空格
            size_t checkPos = pos;
            while (checkPos < sourceCode.length() && std::isspace(sourceCode[checkPos])) {
                checkPos++;
            }
            
            // 检查后面是否有 {
            if (checkPos < sourceCode.length() && sourceCode[checkPos] == '{') {
                // 检查元素内部是否包含CHTL语法
                bool hasCHTLSyntax = false;
                size_t searchPos = checkPos + 1;
                while (searchPos < sourceCode.length() && sourceCode[searchPos] != '}') {
                    if (sourceCode[searchPos] == '@' || sourceCode[searchPos] == '[') {
                        hasCHTLSyntax = true;
                        break;
                    }
                    searchPos++;
                }
                
                // 如果包含CHTL语法，检查是否有嵌套的@Style语法
                if (hasCHTLSyntax) {
                    std::cout << "[UnifiedScanner] Checking for nested @Style syntax in " << identifier << std::endl;
                    searchPos = checkPos + 1;
                    while (searchPos < sourceCode.length() && sourceCode[searchPos] != '}') {
                        if (sourceCode[searchPos] == '@') {
                            std::cout << "[UnifiedScanner] Found @ at position " << searchPos << std::endl;
                            // 检查是否为@Style语法
                            size_t atPos = searchPos;
                            atPos++; // 跳过 @
                            
                            // 检查后面是否有标识符
                            if (atPos < sourceCode.length() && std::isalpha(sourceCode[atPos])) {
                                // 跳过标识符
                                while (atPos < sourceCode.length() && (std::isalnum(sourceCode[atPos]) || sourceCode[atPos] == '_')) {
                                    atPos++;
                                }
                                
                                std::string firstIdentifier = sourceCode.substr(searchPos + 1, atPos - searchPos - 1);
                                std::cout << "[UnifiedScanner] First identifier: " << firstIdentifier << std::endl;
                                
                                // 检查后面是否有空格和另一个标识符
                                if (atPos < sourceCode.length() && sourceCode[atPos] == ' ') {
                                    atPos = skipWhitespace(sourceCode, atPos);
                                    if (atPos < sourceCode.length() && std::isalpha(sourceCode[atPos])) {
                                        size_t secondStart = atPos;
                                        // 跳过第二个标识符
                                        while (atPos < sourceCode.length() && (std::isalnum(sourceCode[atPos]) || sourceCode[atPos] == '_')) {
                                            atPos++;
                                        }
                                        
                                        std::string secondIdentifier = sourceCode.substr(secondStart, atPos - secondStart);
                                        std::cout << "[UnifiedScanner] Second identifier: " << secondIdentifier << std::endl;
                                        
                                        // 跳过空格，然后检查后面是否有 {
                                        atPos = skipWhitespace(sourceCode, atPos);
                                        std::cout << "[UnifiedScanner] Checking for { at position " << atPos << ", char: " << (atPos < sourceCode.length() ? sourceCode[atPos] : '?') << std::endl;
                                        if (atPos < sourceCode.length() && sourceCode[atPos] == '{') {
                                            std::cout << "[UnifiedScanner] Found nested @Style syntax at " << searchPos << std::endl;
                                            // 这里不需要添加边界，因为整个div已经被识别为CHTL片段
                                        } else {
                                            std::cout << "[UnifiedScanner] No { found after @Style" << std::endl;
                                        }
                                    }
                                }
                            }
                        }
                        searchPos++;
                    }
                }
                
                std::cout << "[UnifiedScanner] Element " << identifier 
                          << " hasCHTLSyntax=" << (hasCHTLSyntax ? "true" : "false") << std::endl;
                
                if (hasCHTLSyntax) {
                    std::cout << "[UnifiedScanner] Adding CHTL boundary at " << startPos << std::endl;
                    boundaries.emplace_back(startPos, CodeFragmentType::CHTL);
                } else {
                    std::cout << "[UnifiedScanner] Adding HTML boundary at " << startPos << std::endl;
                    boundaries.emplace_back(startPos, CodeFragmentType::HTML);
                }
                pos = checkPos;
                continue;
            }
            pos = startPos; // 回退
        }
        
        // 检查CHTL关键字语法
        if (sourceCode[pos] == '[') {
            // 检查是否为CHTL关键字
            size_t endPos = pos;
            while (endPos < sourceCode.length() && sourceCode[endPos] != ']') {
                endPos++;
            }
            if (endPos < sourceCode.length()) {
                endPos++; // 包含 ']'
                std::string keyword = sourceCode.substr(pos, endPos - pos);
                if (m_chtlKeywords.find(keyword) != m_chtlKeywords.end()) {
                    boundaries.emplace_back(pos, CodeFragmentType::CHTL);
                    pos = endPos;
                    continue;
                }
            }
        }
        
        // 检查CHTL自定义语法 @Style, @Element, @Var
        if (sourceCode[pos] == '@') {
            size_t startPos = pos;
            pos++; // 跳过 @
            
            // 检查后面是否有标识符
            if (pos < sourceCode.length() && std::isalpha(sourceCode[pos])) {
                // 跳过标识符
                while (pos < sourceCode.length() && (std::isalnum(sourceCode[pos]) || sourceCode[pos] == '_')) {
                    pos++;
                }
                
                // 检查后面是否有空格和另一个标识符
                if (pos < sourceCode.length() && sourceCode[pos] == ' ') {
                    pos = skipWhitespace(sourceCode, pos);
                    if (pos < sourceCode.length() && std::isalpha(sourceCode[pos])) {
                        // 跳过第二个标识符
                        while (pos < sourceCode.length() && (std::isalnum(sourceCode[pos]) || sourceCode[pos] == '_')) {
                            pos++;
                        }
                        
                        // 检查后面是否有 {
                        if (pos < sourceCode.length() && sourceCode[pos] == '{') {
                            std::cout << "[UnifiedScanner] Found @Style syntax at " << startPos << std::endl;
                            boundaries.emplace_back(startPos, CodeFragmentType::CHTL);
                            continue;
                        }
                    }
                }
            }
            pos = startPos; // 回退
        }
        
        
        // 检查CHTL JS语法
        if (sourceCode[pos] == '{' && pos + 1 < sourceCode.length() && sourceCode[pos + 1] == '{') {
            boundaries.emplace_back(pos, CodeFragmentType::CHTL_JS);
            pos += 2;
            continue;
        }
        
        // 检查CSS语法
        if (sourceCode[pos] == '<' && pos + 6 < sourceCode.length() &&
            sourceCode.substr(pos, 7) == "<style>") {
            boundaries.emplace_back(pos, CodeFragmentType::CSS);
            pos += 7;
            continue;
        }
        
        // 检查JS语法
        if (sourceCode[pos] == '<' && pos + 6 < sourceCode.length() &&
            sourceCode.substr(pos, 7) == "<script") {
            boundaries.emplace_back(pos, CodeFragmentType::JS);
            pos += 7;
            continue;
        }
        
        pos++;
    }
    
    return boundaries;
}

int UnifiedScanner::identifyCHTLBoundary(const std::string& sourceCode, size_t startPos) {
    // CHTL语法通常以块形式存在，需要找到匹配的右括号
    if (startPos >= sourceCode.length()) return -1;
    
    size_t pos = startPos;
    
    // 跳过标识符（HTML元素名或CHTL关键字）
    while (pos < sourceCode.length() && (std::isalnum(sourceCode[pos]) || sourceCode[pos] == '-' || sourceCode[pos] == '_')) {
        pos++;
    }
    
    // 跳过空格
    while (pos < sourceCode.length() && std::isspace(sourceCode[pos])) {
        pos++;
    }
    
    if (pos >= sourceCode.length() || sourceCode[pos] != '{') {
        return -1;
    }
    
    // 查找匹配的右括号
    int endPos = findMatchingBracket(sourceCode, pos, '{', '}');
    if (endPos > 0) {
        return endPos + 1; // 包含右括号
    }
    
    return -1;
}

int UnifiedScanner::identifyCHTLJSBoundary(const std::string& sourceCode, size_t startPos) {
    // CHTL JS语法以{{开始，以}}结束
    if (startPos + 1 >= sourceCode.length() || 
        sourceCode[startPos] != '{' || sourceCode[startPos + 1] != '{') {
        return -1;
    }
    
    size_t pos = startPos + 2;
    while (pos + 1 < sourceCode.length()) {
        if (sourceCode[pos] == '}' && sourceCode[pos + 1] == '}') {
            return pos + 2; // 包含}}
        }
        pos++;
    }
    
    return -1;
}

int UnifiedScanner::identifyCSSBoundary(const std::string& sourceCode, size_t startPos) {
    // CSS在<style>标签内
    if (startPos + 6 >= sourceCode.length() ||
        sourceCode.substr(startPos, 7) != "<style>") {
        return -1;
    }
    
    size_t pos = startPos + 7;
    while (pos + 7 < sourceCode.length()) {
        if (sourceCode.substr(pos, 8) == "</style>") {
            return pos + 8; // 包含</style>
        }
        pos++;
    }
    
    return -1;
}

int UnifiedScanner::identifyJSBoundary(const std::string& sourceCode, size_t startPos) {
    // JS在<script>标签内
    if (startPos + 6 >= sourceCode.length() ||
        sourceCode.substr(startPos, 7) != "<script") {
        return -1;
    }
    
    // 找到script标签的结束
    size_t pos = startPos + 7;
    while (pos < sourceCode.length() && sourceCode[pos] != '>') {
        pos++;
    }
    if (pos >= sourceCode.length()) return -1;
    pos++; // 跳过>
    
    // 找到</script>
    while (pos + 8 < sourceCode.length()) {
        if (sourceCode.substr(pos, 9) == "</script>") {
            return pos + 9; // 包含</script>
        }
        pos++;
    }
    
    return -1;
}

std::string UnifiedScanner::replaceWithPlaceholders(const std::string& sourceCode, 
                                                   const std::vector<CodeFragment>& fragments) {
    std::string result = sourceCode;
    
    // 从后往前替换，避免位置偏移
    for (int i = fragments.size() - 1; i >= 0; --i) {
        const auto& fragment = fragments[i];
        size_t startPos = 0;
        size_t currentPos = 0;
        size_t line = 1, column = 1;
        
        // 计算实际位置
        while (currentPos < sourceCode.length() && (line < fragment.line ||
               (line == fragment.line && column < fragment.column))) {
            if (sourceCode[currentPos] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            currentPos++;
        }
        
        if (currentPos < sourceCode.length()) {
            size_t endPos = currentPos + fragment.content.length();
            // Replace with placeholder (simplified for compatibility)
            std::string placeholder = generatePlaceholder(fragment.type, 0);
            result.replace(currentPos, fragment.content.length(), placeholder);
        }
    }
    
    return result;
}

std::string UnifiedScanner::generatePlaceholder(CodeFragmentType type, size_t index) {
    std::ostringstream oss;
    oss << m_placeholderPrefix;
    
    switch (type) {
        case CodeFragmentType::CHTL:
            oss << "CHTL_";
            break;
        case CodeFragmentType::CHTL_JS:
            oss << "CHTLJS_";
            break;
        case CodeFragmentType::CSS:
            oss << "CSS_";
            break;
        case CodeFragmentType::JS:
            oss << "JS_";
            break;
        case CodeFragmentType::HTML:
            oss << "HTML_";
            break;
        default:
            oss << "UNKNOWN_";
            break;
    }
    
    oss << index;
    return oss.str();
}

bool UnifiedScanner::isCHTLKeyword(const std::string& word) const {
    return m_chtlKeywords.find(word) != m_chtlKeywords.end();
}

bool UnifiedScanner::isCHTLJSKeyword(const std::string& word) const {
    return m_chtlJSKeywords.find(word) != m_chtlJSKeywords.end();
}

size_t UnifiedScanner::skipWhitespace(const std::string& sourceCode, size_t pos) const {
    while (pos < sourceCode.length() && std::isspace(sourceCode[pos])) {
        pos++;
    }
    return pos;
}

size_t UnifiedScanner::skipComment(const std::string& sourceCode, size_t pos) const {
    if (pos + 1 < sourceCode.length() && 
        sourceCode[pos] == '/' && sourceCode[pos + 1] == '/') {
        // 单行注释
        while (pos < sourceCode.length() && sourceCode[pos] != '\n') {
            pos++;
        }
    } else if (pos + 1 < sourceCode.length() && 
               sourceCode[pos] == '/' && sourceCode[pos + 1] == '*') {
        // 多行注释
        pos += 2;
        while (pos + 1 < sourceCode.length()) {
            if (sourceCode[pos] == '*' && sourceCode[pos + 1] == '/') {
                pos += 2;
                break;
            }
            pos++;
        }
    }
    return pos;
}

size_t UnifiedScanner::skipString(const std::string& sourceCode, size_t pos, char quote) const {
    if (pos >= sourceCode.length() || sourceCode[pos] != quote) {
        return pos;
    }
    
    pos++; // 跳过开始引号
    while (pos < sourceCode.length()) {
        if (sourceCode[pos] == quote && (pos == 0 || sourceCode[pos - 1] != '\\')) {
            pos++; // 跳过结束引号
            break;
        }
        pos++;
    }
    
    return pos;
}

int UnifiedScanner::findMatchingBracket(const std::string& sourceCode, size_t pos, 
                                       char leftChar, char rightChar) const {
    if (pos >= sourceCode.length() || sourceCode[pos] != leftChar) {
        return -1;
    }
    
    int depth = 1;
    pos++;
    
    while (pos < sourceCode.length() && depth > 0) {
        if (sourceCode[pos] == leftChar) {
            depth++;
        } else if (sourceCode[pos] == rightChar) {
            depth--;
        } else if (sourceCode[pos] == '"' || sourceCode[pos] == '\'') {
            // 跳过字符串
            pos = skipString(sourceCode, pos, sourceCode[pos]);
            continue;
        } else if (pos + 1 < sourceCode.length() && 
                   sourceCode[pos] == '/' && sourceCode[pos + 1] == '/') {
            // 跳过单行注释
            pos = skipComment(sourceCode, pos);
            continue;
        } else if (pos + 1 < sourceCode.length() && 
                   sourceCode[pos] == '/' && sourceCode[pos + 1] == '*') {
            // 跳过多行注释
            pos = skipComment(sourceCode, pos);
            continue;
        }
        
        pos++;
    }
    
    return depth == 0 ? static_cast<int>(pos - 1) : -1;
}

std::pair<size_t, size_t> UnifiedScanner::getLineColumn(const std::string& sourceCode, size_t pos) const {
    size_t line = 1;
    size_t column = 1;
    
    for (size_t i = 0; i < pos && i < sourceCode.length(); ++i) {
        if (sourceCode[i] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
    
    return {line, column};
}

int UnifiedScanner::identifyHTMLBoundary(const std::string& sourceCode, size_t startPos) {
    // HTML元素通常以块形式存在，需要找到匹配的右括号
    if (startPos >= sourceCode.length()) return -1;
    
    size_t pos = startPos;
    
    // 跳过标识符（HTML元素名）
    while (pos < sourceCode.length() && (std::isalnum(sourceCode[pos]) || sourceCode[pos] == '-' || sourceCode[pos] == '_')) {
        pos++;
    }
    
    // 跳过空格
    while (pos < sourceCode.length() && std::isspace(sourceCode[pos])) {
        pos++;
    }
    
    if (pos >= sourceCode.length() || sourceCode[pos] != '{') {
        return -1;
    }
    
    // 查找匹配的右括号
    int endPos = findMatchingBracket(sourceCode, pos, '{', '}');
    if (endPos > 0) {
        return endPos + 1; // 包含右括号
    }
    
    return -1;
}

} // namespace CHTL