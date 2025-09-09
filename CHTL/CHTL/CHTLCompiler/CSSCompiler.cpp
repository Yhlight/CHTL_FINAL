/**
 * @file CSSCompiler.cpp
 * @brief CSS 编译器实现
 * 
 * 根据 CHTL.md 文档实现
 * 提供 CSS 编译和优化功能
 */

#include "CSSCompiler.hpp"
#include <algorithm>
#include <regex>
#include <sstream>
#include <set>

namespace CHTL {

CSSCompiler::CSSCompiler() : minify_(false), optimize_(false) {}

CSSCompiler::~CSSCompiler() = default;

bool CSSCompiler::compile(const std::string& input, std::string& output) {
    try {
        // 处理 CSS 变量
        std::string processed = processVariables(input);
        
        // 优化 CSS（如果启用）
        if (optimize_) {
            processed = optimizeCSS(processed);
        }
        
        // 压缩 CSS（如果启用）
        if (minify_) {
            processed = minifyCSS(processed);
        }
        
        output = processed;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool CSSCompiler::optimize(const std::string& input, std::string& output) {
    try {
        output = optimizeCSS(input);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool CSSCompiler::validate(const std::string& input) {
    try {
        // 基本的 CSS 语法验证
        std::regex cssRule(R"(\s*[^{]+\s*\{\s*[^}]*\s*\})");
        std::smatch match;
        
        std::string content = input;
        while (std::regex_search(content, match, cssRule)) {
            content = match.suffix();
        }
        
        // 检查是否还有未匹配的内容
        content = std::regex_replace(content, std::regex(R"(\s+)"), "");
        return content.empty() || content.find_first_not_of(" \t\n\r") == std::string::npos;
    } catch (const std::exception& e) {
        return false;
    }
}

void CSSCompiler::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string CSSCompiler::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

void CSSCompiler::clearVariables() {
    variables_.clear();
}

std::string CSSCompiler::processVariables(const std::string& input) {
    std::string result = input;
    
    // 处理 CSS 变量替换
    for (const auto& var : variables_) {
        std::string pattern = "var\\(" + var.first + "\\)";
        std::regex regex(pattern);
        result = std::regex_replace(result, regex, var.second);
    }
    
    return result;
}

std::string CSSCompiler::minifyCSS(const std::string& input) {
    std::string result = input;
    
    // 移除注释
    result = std::regex_replace(result, std::regex(R"(\/\*[^*]*\*+(?:[^/*][^*]*\*+)*\/)"), "");
    
    // 移除多余的空白
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除分号前的空白
    result = std::regex_replace(result, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除冒号前后的空白
    result = std::regex_replace(result, std::regex(R"(\s*:\s*)"), ":");
    
    // 移除大括号前后的空白
    result = std::regex_replace(result, std::regex(R"(\s*\{\s*)"), "{");
    result = std::regex_replace(result, std::regex(R"(\s*\}\s*)"), "}");
    
    // 移除逗号后的空白
    result = std::regex_replace(result, std::regex(R"(\s*,\s*)"), ",");
    
    // 移除行首行尾空白
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)", std::regex_constants::multiline), "");
    
    return result;
}

std::string CSSCompiler::optimizeCSS(const std::string& input) {
    std::string result = input;
    
    // 合并相同的选择器
    std::map<std::string, std::vector<std::string>> selectorMap;
    std::regex ruleRegex(R"(([^{]+)\{([^}]+)\})");
    std::sregex_iterator iter(result.begin(), result.end(), ruleRegex);
    std::sregex_iterator end;
    
    std::string optimized;
    std::set<std::string> processedRules;
    
    for (; iter != end; ++iter) {
        std::string selector = iter->str(1);
        std::string properties = iter->str(2);
        
        // 清理选择器和属性
        selector = std::regex_replace(selector, std::regex(R"(\s+)"), " ");
        selector = std::regex_replace(selector, std::regex(R"(^\s+|\s+$)"), "");
        
        properties = std::regex_replace(properties, std::regex(R"(\s+)"), " ");
        properties = std::regex_replace(properties, std::regex(R"(^\s+|\s+$)"), "");
        
        if (processedRules.find(selector) == processedRules.end()) {
            processedRules.insert(selector);
            optimized += selector + "{" + properties + "}";
        }
    }
    
    return optimized.empty() ? result : optimized;
}

} // namespace CHTL