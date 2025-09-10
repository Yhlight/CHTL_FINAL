#include "UnifiedScanner.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <fstream>

namespace CHTL {

// UnifiedScanner 实现
UnifiedScanner::UnifiedScanner() : m_hasError(false) {
    initialize();
}

UnifiedScanner::~UnifiedScanner() {
    cleanup();
}

// 基本扫描功能实现
ScanResult UnifiedScanner::scan(const std::string& code, const ScanConfig& config) {
    ScanResult result;
    result.success = false;
    
    if (!validateInput(code) || !validateConfig(config)) {
        result.error = "Invalid input or configuration";
        return result;
    }
    
    try {
        // 检测语法边界
        if (config.enableBoundaryDetection) {
            result.boundaries = detectBoundaries(code, config);
        }
        
        // 分离代码
        result.fragments = separateCode(code, config);
        
        // 按类型分组片段
        for (const auto& fragment : result.fragments) {
            result.fragmentsByType[fragment.type].push_back(fragment);
        }
        
        // 创建占位符
        if (config.enablePlaceholders) {
            result.placeholders = createPlaceholders(code, config);
            result.processedCode = replacePlaceholders(code, result.placeholders);
        } else {
            result.processedCode = code;
        }
        
        result.success = true;
    } catch (const std::exception& e) {
        result.error = e.what();
        m_lastError = e.what();
        m_hasError = true;
    }
    
    return result;
}

ScanResult UnifiedScanner::scanFile(const std::string& filePath, const ScanConfig& config) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        ScanResult result;
        result.success = false;
        result.error = "Failed to open file: " + filePath;
        return result;
    }
    
    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    return scan(code, config);
}

// 代码分离功能实现
std::vector<CodeFragment> UnifiedScanner::separateCode(const std::string& code, const ScanConfig& config) {
    std::vector<CodeFragment> fragments;
    
    if (config.enableCHTL) {
        auto chtlFragments = separateCHTLCode(code);
        fragments.insert(fragments.end(), chtlFragments.begin(), chtlFragments.end());
    }
    
    if (config.enableCHTLJS) {
        auto chtljsFragments = separateCHTLJSCode(code);
        fragments.insert(fragments.end(), chtljsFragments.begin(), chtljsFragments.end());
    }
    
    if (config.enableJavaScript) {
        auto jsFragments = separateJavaScriptCode(code);
        fragments.insert(fragments.end(), jsFragments.begin(), jsFragments.end());
    }
    
    if (config.enableCSS) {
        auto cssFragments = separateCSSCode(code);
        fragments.insert(fragments.end(), cssFragments.begin(), cssFragments.end());
    }
    
    if (config.enableHTML) {
        auto htmlFragments = separateHTMLCode(code);
        fragments.insert(fragments.end(), htmlFragments.begin(), htmlFragments.end());
    }
    
    return fragments;
}

std::map<CodeType, std::vector<CodeFragment>> UnifiedScanner::separateCodeByType(const std::string& code, const ScanConfig& config) {
    auto fragments = separateCode(code, config);
    std::map<CodeType, std::vector<CodeFragment>> result;
    
    for (const auto& fragment : fragments) {
        result[fragment.type].push_back(fragment);
    }
    
    return result;
}

// 语法边界检测实现
std::vector<SyntaxBoundary> UnifiedScanner::detectBoundaries(const std::string& code, const ScanConfig& config) {
    std::vector<SyntaxBoundary> boundaries;
    
    if (config.enableCHTL) {
        auto chtlBoundaries = detectCHTLBoundaries(code);
        boundaries.insert(boundaries.end(), chtlBoundaries.begin(), chtlBoundaries.end());
    }
    
    if (config.enableCHTLJS) {
        auto chtljsBoundaries = detectCHTLJSBoundaries(code);
        boundaries.insert(boundaries.end(), chtljsBoundaries.begin(), chtljsBoundaries.end());
    }
    
    if (config.enableJavaScript) {
        auto jsBoundaries = detectJavaScriptBoundaries(code);
        boundaries.insert(boundaries.end(), jsBoundaries.begin(), jsBoundaries.end());
    }
    
    if (config.enableCSS) {
        auto cssBoundaries = detectCSSBoundaries(code);
        boundaries.insert(boundaries.end(), cssBoundaries.begin(), cssBoundaries.end());
    }
    
    if (config.enableHTML) {
        auto htmlBoundaries = detectHTMLBoundaries(code);
        boundaries.insert(boundaries.end(), htmlBoundaries.begin(), htmlBoundaries.end());
    }
    
    if (config.enableComments) {
        auto commentBoundaries = detectCommentBoundaries(code);
        boundaries.insert(boundaries.end(), commentBoundaries.begin(), commentBoundaries.end());
    }
    
    if (config.enableStrings) {
        auto stringBoundaries = detectStringBoundaries(code);
        boundaries.insert(boundaries.end(), stringBoundaries.begin(), stringBoundaries.end());
    }
    
    if (config.enableBrackets) {
        auto bracketBoundaries = detectBracketBoundaries(code);
        boundaries.insert(boundaries.end(), bracketBoundaries.begin(), bracketBoundaries.end());
    }
    
    return boundaries;
}

// 占位符管理实现
std::vector<Placeholder> UnifiedScanner::createPlaceholders(const std::string& code, const ScanConfig& config) {
    std::vector<Placeholder> placeholders;
    
    // 为不同类型的代码创建占位符
    auto fragments = separateCode(code, config);
    size_t index = 0;
    
    for (const auto& fragment : fragments) {
        if (fragment.type == CodeType::JAVASCRIPT || fragment.type == CodeType::CSS) {
            std::string id = generatePlaceholderId(index++, fragment.type);
            Placeholder placeholder(id, fragment.content, fragment.type, fragment.startPos, fragment.endPos - fragment.startPos);
            placeholders.push_back(placeholder);
        }
    }
    
    return placeholders;
}

std::string UnifiedScanner::replacePlaceholders(const std::string& code, const std::vector<Placeholder>& placeholders) {
    std::string result = code;
    
    for (const auto& placeholder : placeholders) {
        std::string placeholderText = "_" + codeTypeToString(placeholder.type) + "_CODE_PLACEHOLDER_";
        result.replace(placeholder.position, placeholder.length, placeholderText);
    }
    
    return result;
}

std::string UnifiedScanner::restorePlaceholders(const std::string& code, const std::vector<Placeholder>& placeholders) {
    std::string result = code;
    
    for (const auto& placeholder : placeholders) {
        std::string placeholderText = "_" + codeTypeToString(placeholder.type) + "_CODE_PLACEHOLDER_";
        size_t pos = result.find(placeholderText);
        if (pos != std::string::npos) {
            result.replace(pos, placeholderText.length(), placeholder.content);
        }
    }
    
    return result;
}

// 代码类型检测实现
CodeType UnifiedScanner::detectCodeType(const std::string& code, size_t position, const ScanConfig& config) {
    if (config.enableCHTL && detectCHTLType(code, position) != CodeType::UNKNOWN) {
        return CodeType::CHTL;
    }
    
    if (config.enableCHTLJS && detectCHTLJSType(code, position) != CodeType::UNKNOWN) {
        return CodeType::CHTL_JS;
    }
    
    if (config.enableJavaScript && detectJavaScriptType(code, position) != CodeType::UNKNOWN) {
        return CodeType::JAVASCRIPT;
    }
    
    if (config.enableCSS && detectCSSType(code, position) != CodeType::UNKNOWN) {
        return CodeType::CSS;
    }
    
    if (config.enableHTML && detectHTMLType(code, position) != CodeType::UNKNOWN) {
        return CodeType::HTML;
    }
    
    return CodeType::UNKNOWN;
}

// 代码验证实现
bool UnifiedScanner::validateCode(const std::string& code, const ScanConfig& config) {
    if (!validateInput(code)) {
        return false;
    }
    
    if (config.enableCHTL && !validateCHTLCode(code)) {
        return false;
    }
    
    if (config.enableCHTLJS && !validateCHTLJSCode(code)) {
        return false;
    }
    
    if (config.enableJavaScript && !validateJavaScriptCode(code)) {
        return false;
    }
    
    if (config.enableCSS && !validateCSSCode(code)) {
        return false;
    }
    
    if (config.enableHTML && !validateHTMLCode(code)) {
        return false;
    }
    
    return true;
}

// 工具函数实现
std::string UnifiedScanner::codeTypeToString(CodeType type) {
    switch (type) {
        case CodeType::CHTL: return "CHTL";
        case CodeType::CHTL_JS: return "CHTL_JS";
        case CodeType::JAVASCRIPT: return "JAVASCRIPT";
        case CodeType::CSS: return "CSS";
        case CodeType::HTML: return "HTML";
        default: return "UNKNOWN";
    }
}

CodeType UnifiedScanner::stringToCodeType(const std::string& type) {
    if (type == "CHTL") return CodeType::CHTL;
    if (type == "CHTL_JS") return CodeType::CHTL_JS;
    if (type == "JAVASCRIPT") return CodeType::JAVASCRIPT;
    if (type == "CSS") return CodeType::CSS;
    if (type == "HTML") return CodeType::HTML;
    return CodeType::UNKNOWN;
}

// 内部方法实现
void UnifiedScanner::initialize() {
    initializePatterns();
    m_hasError = false;
    m_lastError.clear();
}

void UnifiedScanner::cleanup() {
    cleanupPatterns();
    m_fragments.clear();
    m_boundaries.clear();
    m_placeholders.clear();
}

bool UnifiedScanner::validateInput(const std::string& code) const {
    return !code.empty();
}

bool UnifiedScanner::validateConfig(const ScanConfig& config) const {
    // 简单验证，至少启用一种类型
    return config.enableCHTL || config.enableCHTLJS || config.enableJavaScript || 
           config.enableCSS || config.enableHTML;
}

void UnifiedScanner::initializePatterns() {
    // 初始化正则表达式模式
    try {
        m_chtlPattern = std::regex(R"(\[.*?\]|\{.*?\})");
        m_chtljsPattern = std::regex(R"(\{\{.*?\}\})");
        m_javascriptPattern = std::regex(R"(function\s+\w+|var\s+\w+|let\s+\w+|const\s+\w+)");
        m_cssPattern = std::regex(R"(\w+\s*:\s*[^;]+;)");
        m_htmlPattern = std::regex(R"(<[^>]+>)");
        m_commentPattern = std::regex(R"(//.*|/\*.*?\*/|--.*?)");
        m_stringPattern = std::regex(R"(".*?"|'.*?'|`.*?`)");
        m_bracketPattern = std::regex(R"(\(|\)|\[|\]|\{|\})");
    } catch (const std::exception& e) {
        m_lastError = "Failed to initialize patterns: " + std::string(e.what());
        m_hasError = true;
    }
}

void UnifiedScanner::cleanupPatterns() {
    // 清理正则表达式模式（C++中regex析构函数会自动处理）
}

// 边界检测内部方法（简化实现）
std::vector<SyntaxBoundary> UnifiedScanner::detectCHTLBoundaries(const std::string& code) {
    std::vector<SyntaxBoundary> boundaries;
    // 简化实现：检测CHTL块语法
    size_t pos = 0;
    while ((pos = code.find('[', pos)) != std::string::npos) {
        size_t end = code.find(']', pos);
        if (end != std::string::npos) {
            boundaries.emplace_back(SyntaxBoundaryType::CHTL_START, pos, end - pos + 1, 
                                  code.substr(pos, end - pos + 1));
            pos = end + 1;
        } else {
            break;
        }
    }
    return boundaries;
}

std::vector<SyntaxBoundary> UnifiedScanner::detectCHTLJSBoundaries(const std::string& code) {
    std::vector<SyntaxBoundary> boundaries;
    // 简化实现：检测CHTL JS语法
    size_t pos = 0;
    while ((pos = code.find("{{", pos)) != std::string::npos) {
        size_t end = code.find("}}", pos);
        if (end != std::string::npos) {
            boundaries.emplace_back(SyntaxBoundaryType::CHTL_JS_START, pos, end - pos + 2, 
                                  code.substr(pos, end - pos + 2));
            pos = end + 2;
        } else {
            break;
        }
    }
    return boundaries;
}

std::vector<SyntaxBoundary> UnifiedScanner::detectJavaScriptBoundaries(const std::string& code) {
    std::vector<SyntaxBoundary> boundaries;
    // 简化实现：检测JavaScript函数
    size_t pos = 0;
    while ((pos = code.find("function", pos)) != std::string::npos) {
        boundaries.emplace_back(SyntaxBoundaryType::JAVASCRIPT_START, pos, 8, "function");
        pos += 8;
    }
    return boundaries;
}

std::vector<SyntaxBoundary> UnifiedScanner::detectCSSBoundaries(const std::string& code) {
    std::vector<SyntaxBoundary> boundaries;
    // 简化实现：检测CSS规则
    size_t pos = 0;
    while ((pos = code.find('{', pos)) != std::string::npos) {
        size_t end = code.find('}', pos);
        if (end != std::string::npos) {
            boundaries.emplace_back(SyntaxBoundaryType::CSS_START, pos, end - pos + 1, 
                                  code.substr(pos, end - pos + 1));
            pos = end + 1;
        } else {
            break;
        }
    }
    return boundaries;
}

std::vector<SyntaxBoundary> UnifiedScanner::detectHTMLBoundaries(const std::string& code) {
    std::vector<SyntaxBoundary> boundaries;
    // 简化实现：检测HTML标签
    size_t pos = 0;
    while ((pos = code.find('<', pos)) != std::string::npos) {
        size_t end = code.find('>', pos);
        if (end != std::string::npos) {
            boundaries.emplace_back(SyntaxBoundaryType::HTML_START, pos, end - pos + 1, 
                                  code.substr(pos, end - pos + 1));
            pos = end + 1;
        } else {
            break;
        }
    }
    return boundaries;
}

std::vector<SyntaxBoundary> UnifiedScanner::detectCommentBoundaries(const std::string& code) {
    std::vector<SyntaxBoundary> boundaries;
    // 简化实现：检测注释
    size_t pos = 0;
    while ((pos = code.find("//", pos)) != std::string::npos) {
        size_t end = code.find('\n', pos);
        if (end == std::string::npos) end = code.length();
        boundaries.emplace_back(SyntaxBoundaryType::COMMENT_START, pos, end - pos, 
                              code.substr(pos, end - pos));
        pos = end;
    }
    return boundaries;
}

std::vector<SyntaxBoundary> UnifiedScanner::detectStringBoundaries(const std::string& code) {
    std::vector<SyntaxBoundary> boundaries;
    // 简化实现：检测字符串
    size_t pos = 0;
    while ((pos = code.find('"', pos)) != std::string::npos) {
        size_t end = code.find('"', pos + 1);
        if (end != std::string::npos) {
            boundaries.emplace_back(SyntaxBoundaryType::STRING_START, pos, end - pos + 1, 
                                  code.substr(pos, end - pos + 1));
            pos = end + 1;
        } else {
            break;
        }
    }
    return boundaries;
}

std::vector<SyntaxBoundary> UnifiedScanner::detectBracketBoundaries(const std::string& code) {
    std::vector<SyntaxBoundary> boundaries;
    // 简化实现：检测括号
    for (size_t i = 0; i < code.length(); ++i) {
        char ch = code[i];
        if (ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}') {
            boundaries.emplace_back(SyntaxBoundaryType::BRACKET_START, i, 1, std::string(1, ch));
        }
    }
    return boundaries;
}

// 代码分离内部方法（简化实现）
std::vector<CodeFragment> UnifiedScanner::separateCHTLCode(const std::string& code) {
    std::vector<CodeFragment> fragments;
    auto boundaries = detectCHTLBoundaries(code);
    
    for (const auto& boundary : boundaries) {
        CodeFragment fragment(boundary.content, CodeType::CHTL, boundary.position, 
                            boundary.position + boundary.length);
        fragments.push_back(fragment);
    }
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::separateCHTLJSCode(const std::string& code) {
    std::vector<CodeFragment> fragments;
    auto boundaries = detectCHTLJSBoundaries(code);
    
    for (const auto& boundary : boundaries) {
        CodeFragment fragment(boundary.content, CodeType::CHTL_JS, boundary.position, 
                            boundary.position + boundary.length);
        fragments.push_back(fragment);
    }
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::separateJavaScriptCode(const std::string& code) {
    std::vector<CodeFragment> fragments;
    auto boundaries = detectJavaScriptBoundaries(code);
    
    for (const auto& boundary : boundaries) {
        CodeFragment fragment(boundary.content, CodeType::JAVASCRIPT, boundary.position, 
                            boundary.position + boundary.length);
        fragments.push_back(fragment);
    }
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::separateCSSCode(const std::string& code) {
    std::vector<CodeFragment> fragments;
    auto boundaries = detectCSSBoundaries(code);
    
    for (const auto& boundary : boundaries) {
        CodeFragment fragment(boundary.content, CodeType::CSS, boundary.position, 
                            boundary.position + boundary.length);
        fragments.push_back(fragment);
    }
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::separateHTMLCode(const std::string& code) {
    std::vector<CodeFragment> fragments;
    auto boundaries = detectHTMLBoundaries(code);
    
    for (const auto& boundary : boundaries) {
        CodeFragment fragment(boundary.content, CodeType::HTML, boundary.position, 
                            boundary.position + boundary.length);
        fragments.push_back(fragment);
    }
    
    return fragments;
}

// 占位符管理内部方法
std::string UnifiedScanner::generatePlaceholderId(size_t index, CodeType type) {
    return "_" + codeTypeToString(type) + "_PLACEHOLDER_" + std::to_string(index) + "_";
}

// 代码类型检测内部方法（简化实现）
CodeType UnifiedScanner::detectCHTLType(const std::string& code, size_t position) {
    if (position < code.length() && code[position] == '[') {
        return CodeType::CHTL;
    }
    return CodeType::UNKNOWN;
}

CodeType UnifiedScanner::detectCHTLJSType(const std::string& code, size_t position) {
    if (position < code.length() - 1 && code.substr(position, 2) == "{{") {
        return CodeType::CHTL_JS;
    }
    return CodeType::UNKNOWN;
}

CodeType UnifiedScanner::detectJavaScriptType(const std::string& code, size_t position) {
    std::string substr = code.substr(position, 8);
    if (substr == "function" || substr.substr(0, 3) == "var" || substr.substr(0, 3) == "let") {
        return CodeType::JAVASCRIPT;
    }
    return CodeType::UNKNOWN;
}

CodeType UnifiedScanner::detectCSSType(const std::string& code, size_t position) {
    // 简化检测：查找CSS属性模式
    size_t colonPos = code.find(':', position);
    size_t semicolonPos = code.find(';', position);
    if (colonPos != std::string::npos && semicolonPos != std::string::npos && colonPos < semicolonPos) {
        return CodeType::CSS;
    }
    return CodeType::UNKNOWN;
}

CodeType UnifiedScanner::detectHTMLType(const std::string& code, size_t position) {
    if (position < code.length() && code[position] == '<') {
        return CodeType::HTML;
    }
    return CodeType::UNKNOWN;
}

// 代码验证内部方法（简化实现）
bool UnifiedScanner::validateCHTLCode(const std::string& code) {
    // 简化验证：检查括号匹配
    int bracketCount = 0;
    for (char ch : code) {
        if (ch == '[') bracketCount++;
        if (ch == ']') bracketCount--;
    }
    return bracketCount == 0;
}

bool UnifiedScanner::validateCHTLJSCode(const std::string& code) {
    // 简化验证：检查双大括号匹配
    size_t openCount = 0;
    size_t closeCount = 0;
    size_t pos = 0;
    while ((pos = code.find("{{", pos)) != std::string::npos) {
        openCount++;
        pos += 2;
    }
    pos = 0;
    while ((pos = code.find("}}", pos)) != std::string::npos) {
        closeCount++;
        pos += 2;
    }
    return openCount == closeCount;
}

bool UnifiedScanner::validateJavaScriptCode(const std::string& code) {
    // 简化验证：检查基本语法
    return code.find("function") != std::string::npos || 
           code.find("var") != std::string::npos ||
           code.find("let") != std::string::npos ||
           code.find("const") != std::string::npos;
}

bool UnifiedScanner::validateCSSCode(const std::string& code) {
    // 简化验证：检查CSS属性
    return code.find(':') != std::string::npos && code.find(';') != std::string::npos;
}

bool UnifiedScanner::validateHTMLCode(const std::string& code) {
    // 简化验证：检查HTML标签
    return code.find('<') != std::string::npos && code.find('>') != std::string::npos;
}

// 状态管理实现
void UnifiedScanner::reset() {
    cleanup();
    initialize();
}

void UnifiedScanner::clear() {
    m_fragments.clear();
    m_boundaries.clear();
    m_placeholders.clear();
    m_lastError.clear();
    m_hasError = false;
}

bool UnifiedScanner::isEmpty() const {
    return m_fragments.empty() && m_boundaries.empty() && m_placeholders.empty();
}

size_t UnifiedScanner::getFragmentCount() const {
    return m_fragments.size();
}

size_t UnifiedScanner::getBoundaryCount() const {
    return m_boundaries.size();
}

size_t UnifiedScanner::getPlaceholderCount() const {
    return m_placeholders.size();
}

// 错误处理实现
std::string UnifiedScanner::getLastError() const {
    return m_lastError;
}

void UnifiedScanner::clearError() {
    m_lastError.clear();
    m_hasError = false;
}

bool UnifiedScanner::hasError() const {
    return m_hasError;
}

// 配置管理实现
void UnifiedScanner::setConfig(const ScanConfig& config) {
    m_config = config;
}

ScanConfig UnifiedScanner::getConfig() const {
    return m_config;
}

void UnifiedScanner::resetConfig() {
    m_config = ScanConfig{};
}

} // namespace CHTL