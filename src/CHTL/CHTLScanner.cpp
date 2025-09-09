#include "CHTL/CHTLScanner.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// Placeholder 实现
Placeholder::Placeholder(const std::string& content, PlaceholderType type, int start, int end)
    : m_content(content), m_type(type), m_start(start), m_end(end) {
}

bool Placeholder::isValid() const {
    return !m_content.empty() && m_start >= 0 && m_end > m_start;
}

bool Placeholder::isVariable() const {
    return m_type == PlaceholderType::VARIABLE;
}

bool Placeholder::isFunction() const {
    return m_type == PlaceholderType::FUNCTION;
}

bool Placeholder::isExpression() const {
    return m_type == PlaceholderType::EXPRESSION;
}

bool Placeholder::isBlock() const {
    return m_type == PlaceholderType::BLOCK;
}

// CodeFragment 实现
CodeFragment::CodeFragment(const std::string& content, int start, int end, bool isPlaceholder)
    : m_content(content), m_start(start), m_end(end), m_isPlaceholder(isPlaceholder) {
}

bool CodeFragment::isValid() const {
    return !m_content.empty() && m_start >= 0 && m_end > m_start;
}

bool CodeFragment::isEmpty() const {
    return m_content.empty();
}

// UnifiedScanner 实现
UnifiedScanner::UnifiedScanner(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_scanMode(ScanMode::BROAD) {
    
    // 初始化默认分隔符
    m_delimiters = {" ", "\t", "\n", "\r", ";", ",", "(", ")", "{", "}", "[", "]", "<", ">", "=", "+", "-", "*", "/", "%", "&", "|", "^", "!", "?", ":", "."};
    
    // 初始化占位符模式
    m_placeholderPatterns[PlaceholderType::VARIABLE] = R"(\$\w+)";
    m_placeholderPatterns[PlaceholderType::FUNCTION] = R"(\$\w+\s*\()";
    m_placeholderPatterns[PlaceholderType::EXPRESSION] = R"(\$\{[^}]+\})";
    m_placeholderPatterns[PlaceholderType::BLOCK] = R"(\$\{[^}]*\{[^}]*\}[^}]*\})";
}

std::vector<CodeFragment> UnifiedScanner::scanCode(const std::string& code, ScanMode mode) {
    m_scanMode = mode;
    
    if (mode == ScanMode::BROAD) {
        return scanCodeBroad(code);
    } else {
        return scanCodeStrict(code);
    }
}

std::vector<Placeholder> UnifiedScanner::scanPlaceholders(const std::string& code) {
    if (m_scanMode == ScanMode::BROAD) {
        return scanPlaceholdersBroad(code);
    } else {
        return scanPlaceholdersStrict(code);
    }
}

std::vector<CodeFragment> UnifiedScanner::cutCode(const std::string& code, const std::vector<std::string>& delimiters) {
    std::vector<CodeFragment> fragments;
    
    if (code.empty()) {
        return fragments;
    }
    
    int start = 0;
    int pos = 0;
    
    while (pos < static_cast<int>(code.length())) {
        bool foundDelimiter = false;
        
        for (const auto& delimiter : delimiters) {
            if (pos + delimiter.length() <= static_cast<int>(code.length()) &&
                code.substr(pos, delimiter.length()) == delimiter) {
                
                if (pos > start) {
                    std::string content = code.substr(start, pos - start);
                    fragments.emplace_back(content, start, pos, false);
                }
                
                // 添加分隔符作为片段
                fragments.emplace_back(delimiter, pos, pos + delimiter.length(), false);
                
                start = pos + delimiter.length();
                pos = start;
                foundDelimiter = true;
                break;
            }
        }
        
        if (!foundDelimiter) {
            pos++;
        }
    }
    
    // 添加最后一个片段
    if (start < static_cast<int>(code.length())) {
        std::string content = code.substr(start);
        fragments.emplace_back(content, start, static_cast<int>(code.length()), false);
    }
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::cutCodeByPattern(const std::string& code, const std::string& pattern) {
    std::vector<CodeFragment> fragments;
    
    if (code.empty() || pattern.empty()) {
        return fragments;
    }
    
    std::regex regexPattern(pattern);
    std::sregex_iterator iter(code.begin(), code.end(), regexPattern);
    std::sregex_iterator end;
    
    int lastEnd = 0;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        int matchStart = static_cast<int>(match.position());
        int matchEnd = matchStart + static_cast<int>(match.length());
        
        // 添加匹配前的片段
        if (matchStart > lastEnd) {
            std::string content = code.substr(lastEnd, matchStart - lastEnd);
            fragments.emplace_back(content, lastEnd, matchStart, false);
        }
        
        // 添加匹配的片段
        std::string content = match.str();
        fragments.emplace_back(content, matchStart, matchEnd, false);
        
        lastEnd = matchEnd;
    }
    
    // 添加最后一个片段
    if (lastEnd < static_cast<int>(code.length())) {
        std::string content = code.substr(lastEnd);
        fragments.emplace_back(content, lastEnd, static_cast<int>(code.length()), false);
    }
    
    return fragments;
}

std::string UnifiedScanner::enhanceCode(const std::string& code, ScanMode mode) {
    m_scanMode = mode;
    
    std::string result = code;
    
    // 处理占位符
    auto placeholders = scanPlaceholders(result);
    for (const auto& placeholder : placeholders) {
        std::string enhanced = enhanceFragment(CodeFragment(placeholder.getContent(), placeholder.getStart(), placeholder.getEnd(), true));
        result.replace(placeholder.getStart(), placeholder.getLength(), enhanced);
    }
    
    // 处理边界
    result = processBoundaries(result);
    
    return result;
}

std::string UnifiedScanner::enhanceFragment(const CodeFragment& fragment) {
    if (!fragment.isValid() || fragment.isEmpty()) {
        return fragment.getContent();
    }
    
    std::string content = fragment.getContent();
    
    // 根据占位符类型进行增强
    PlaceholderType type = detectPlaceholderType(content);
    
    switch (type) {
        case PlaceholderType::VARIABLE:
            return processVariablePlaceholder(content);
        case PlaceholderType::FUNCTION:
            return processFunctionPlaceholder(content);
        case PlaceholderType::EXPRESSION:
            return processExpressionPlaceholder(content);
        case PlaceholderType::BLOCK:
            return processBlockPlaceholder(content);
        default:
            return content;
    }
}

std::string UnifiedScanner::createPlaceholder(const std::string& content, PlaceholderType type) {
    std::string id = generatePlaceholderId(content);
    return "$" + id + "[" + std::to_string(static_cast<int>(type)) + "]";
}

std::string UnifiedScanner::replacePlaceholder(const std::string& code, const std::string& placeholder, const std::string& replacement) {
    std::string result = code;
    
    size_t pos = 0;
    while ((pos = result.find(placeholder, pos)) != std::string::npos) {
        result.replace(pos, placeholder.length(), replacement);
        pos += replacement.length();
    }
    
    return result;
}

std::vector<std::string> UnifiedScanner::extractPlaceholders(const std::string& code) {
    std::vector<std::string> placeholders;
    
    for (const auto& [type, pattern] : m_placeholderPatterns) {
        std::regex regexPattern(pattern);
        std::sregex_iterator iter(code.begin(), code.end(), regexPattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            placeholders.push_back(iter->str());
        }
    }
    
    return placeholders;
}

std::string UnifiedScanner::processBoundaries(const std::string& code) {
    std::string result = code;
    
    // 修复语法边界
    result = fixSyntaxBoundaries(result);
    
    // 修复语义边界
    result = fixSemanticBoundaries(result);
    
    // 修复上下文边界
    result = fixContextBoundaries(result);
    
    return result;
}

std::string UnifiedScanner::fixBoundaries(const std::string& code) {
    return processBoundaries(code);
}

std::vector<CodeFragment> UnifiedScanner::scanCodeBroad(const std::string& code) const {
    std::vector<CodeFragment> fragments;
    
    if (code.empty()) {
        return fragments;
    }
    
    // 宽泛模式：按空格和常见分隔符切割
    std::vector<std::string> broadDelimiters = {" ", "\t", "\n", "\r", ";", ",", "(", ")", "{", "}", "[", "]"};
    
    int start = 0;
    int pos = 0;
    
    while (pos < static_cast<int>(code.length())) {
        bool foundDelimiter = false;
        
        for (const auto& delimiter : broadDelimiters) {
            if (pos + delimiter.length() <= static_cast<int>(code.length()) &&
                code.substr(pos, delimiter.length()) == delimiter) {
                
                if (pos > start) {
                    std::string content = code.substr(start, pos - start);
                    fragments.emplace_back(content, start, pos, false);
                }
                
                start = pos + delimiter.length();
                pos = start;
                foundDelimiter = true;
                break;
            }
        }
        
        if (!foundDelimiter) {
            pos++;
        }
    }
    
    // 添加最后一个片段
    if (start < static_cast<int>(code.length())) {
        std::string content = code.substr(start);
        fragments.emplace_back(content, start, static_cast<int>(code.length()), false);
    }
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::scanCodeStrict(const std::string& code) const {
    std::vector<CodeFragment> fragments;
    
    if (code.empty()) {
        return fragments;
    }
    
    // 严格模式：按语法规则切割
    std::vector<std::string> strictDelimiters = {";", "{", "}", "(", ")", "[", "]", ",", ":", "=", "+", "-", "*", "/", "%", "&", "|", "^", "!", "?", "<", ">"};
    
    int start = 0;
    int pos = 0;
    
    while (pos < static_cast<int>(code.length())) {
        bool foundDelimiter = false;
        
        for (const auto& delimiter : strictDelimiters) {
            if (pos + delimiter.length() <= static_cast<int>(code.length()) &&
                code.substr(pos, delimiter.length()) == delimiter) {
                
                if (pos > start) {
                    std::string content = code.substr(start, pos - start);
                    fragments.emplace_back(content, start, pos, false);
                }
                
                start = pos + delimiter.length();
                pos = start;
                foundDelimiter = true;
                break;
            }
        }
        
        if (!foundDelimiter) {
            pos++;
        }
    }
    
    // 添加最后一个片段
    if (start < static_cast<int>(code.length())) {
        std::string content = code.substr(start);
        fragments.emplace_back(content, start, static_cast<int>(code.length()), false);
    }
    
    return fragments;
}

std::vector<Placeholder> UnifiedScanner::scanPlaceholdersBroad(const std::string& code) const {
    std::vector<Placeholder> placeholders;
    
    for (const auto& [type, pattern] : m_placeholderPatterns) {
        std::regex regexPattern(pattern);
        std::sregex_iterator iter(code.begin(), code.end(), regexPattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            int start = static_cast<int>(match.position());
            int end = start + static_cast<int>(match.length());
            std::string content = match.str();
            
            placeholders.emplace_back(content, type, start, end);
        }
    }
    
    return placeholders;
}

std::vector<Placeholder> UnifiedScanner::scanPlaceholdersStrict(const std::string& code) const {
    std::vector<Placeholder> placeholders;
    
    // 严格模式：只识别明确的占位符模式
    std::string strictPattern = R"(\$\w+(?:\[[^\]]+\])?)";
    std::regex regexPattern(strictPattern);
    std::sregex_iterator iter(code.begin(), code.end(), regexPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        int start = static_cast<int>(match.position());
        int end = start + static_cast<int>(match.length());
        std::string content = match.str();
        
        PlaceholderType type = detectPlaceholderType(content);
        placeholders.emplace_back(content, type, start, end);
    }
    
    return placeholders;
}

PlaceholderType UnifiedScanner::detectPlaceholderType(const std::string& content) const {
    if (content.empty()) {
        return PlaceholderType::UNKNOWN;
    }
    
    // 检测变量占位符
    if (std::regex_match(content, std::regex(R"(\$\w+)"))) {
        return PlaceholderType::VARIABLE;
    }
    
    // 检测函数占位符
    if (std::regex_match(content, std::regex(R"(\$\w+\s*\()"))) {
        return PlaceholderType::FUNCTION;
    }
    
    // 检测表达式占位符
    if (std::regex_match(content, std::regex(R"(\$\{[^}]+\})"))) {
        return PlaceholderType::EXPRESSION;
    }
    
    // 检测代码块占位符
    if (std::regex_match(content, std::regex(R"(\$\{[^}]*\{[^}]*\}[^}]*\})"))) {
        return PlaceholderType::BLOCK;
    }
    
    return PlaceholderType::UNKNOWN;
}

std::string UnifiedScanner::generatePlaceholderId(const std::string& content) const {
    // 生成基于内容的唯一ID
    std::hash<std::string> hasher;
    size_t hash = hasher(content);
    return "ph_" + std::to_string(hash);
}

bool UnifiedScanner::isPlaceholderPattern(const std::string& content, PlaceholderType type) const {
    auto it = m_placeholderPatterns.find(type);
    if (it == m_placeholderPatterns.end()) {
        return false;
    }
    
    std::regex pattern(it->second);
    return std::regex_match(content, pattern);
}

std::string UnifiedScanner::processVariablePlaceholder(const std::string& content) const {
    // 处理变量占位符
    std::string result = content;
    
    // 移除 $ 前缀
    if (result.starts_with("$")) {
        result = result.substr(1);
    }
    
    // 添加变量声明
    return "var " + result + ";";
}

std::string UnifiedScanner::processFunctionPlaceholder(const std::string& content) const {
    // 处理函数占位符
    std::string result = content;
    
    // 移除 $ 前缀
    if (result.starts_with("$")) {
        result = result.substr(1);
    }
    
    // 添加函数声明
    return "function " + result + " {}";
}

std::string UnifiedScanner::processExpressionPlaceholder(const std::string& content) const {
    // 处理表达式占位符
    std::string result = content;
    
    // 移除 ${} 包装
    if (result.starts_with("${") && result.ends_with("}")) {
        result = result.substr(2, result.length() - 3);
    }
    
    return result;
}

std::string UnifiedScanner::processBlockPlaceholder(const std::string& content) const {
    // 处理代码块占位符
    std::string result = content;
    
    // 移除 ${} 包装
    if (result.starts_with("${") && result.ends_with("}")) {
        result = result.substr(2, result.length() - 3);
    }
    
    return result;
}

std::string UnifiedScanner::fixSyntaxBoundaries(const std::string& code) const {
    std::string result = code;
    
    // 修复常见的语法边界问题
    // 添加缺失的分号
    std::regex semicolonRegex(R"([^;\s]\s*\n)");
    result = std::regex_replace(result, semicolonRegex, ";\n");
    
    // 修复括号匹配
    int openBraces = 0;
    int openParens = 0;
    int openBrackets = 0;
    
    for (char c : result) {
        switch (c) {
            case '{': openBraces++; break;
            case '}': openBraces--; break;
            case '(': openParens++; break;
            case ')': openParens--; break;
            case '[': openBrackets++; break;
            case ']': openBrackets--; break;
        }
    }
    
    // 添加缺失的闭合括号
    while (openBraces > 0) {
        result += "}";
        openBraces--;
    }
    while (openParens > 0) {
        result += ")";
        openParens--;
    }
    while (openBrackets > 0) {
        result += "]";
        openBrackets--;
    }
    
    return result;
}

std::string UnifiedScanner::fixSemanticBoundaries(const std::string& code) const {
    std::string result = code;
    
    // 修复语义边界问题
    // 确保语句之间有适当的分隔
    std::regex statementRegex(R"([^;\s]\s*[a-zA-Z_])");
    result = std::regex_replace(result, statementRegex, ";\n$&");
    
    return result;
}

std::string UnifiedScanner::fixContextBoundaries(const std::string& code) const {
    std::string result = code;
    
    // 修复上下文边界问题
    // 确保代码块有适当的缩进
    std::stringstream ss(result);
    std::string line;
    std::stringstream output;
    int indentLevel = 0;
    
    while (std::getline(ss, line)) {
        // 计算缩进
        std::string indent(indentLevel * 2, ' ');
        output << indent << line << "\n";
        
        // 更新缩进级别
        for (char c : line) {
            if (c == '{') {
                indentLevel++;
            } else if (c == '}') {
                indentLevel--;
                if (indentLevel < 0) indentLevel = 0;
            }
        }
    }
    
    return output.str();
}

} // namespace CHTL