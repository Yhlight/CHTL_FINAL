#include "CHTL/CHTLCJMODAPI.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <filesystem>

namespace CHTL {

// SyntaxAnalysisResult 实现
SyntaxAnalysisResult::SyntaxAnalysisResult(bool isValid, const std::string& errorMessage)
    : m_isValid(isValid), m_errorMessage(errorMessage) {
}

std::any ParameterBindingResult::getParameter(const std::string& name) const {
    auto it = m_boundParameters.find(name);
    return (it != m_boundParameters.end()) ? it->second : std::any();
}

// CodeGenerationResult 实现
CodeGenerationResult::CodeGenerationResult(bool isSuccess, const std::string& errorMessage)
    : m_isSuccess(isSuccess), m_errorMessage(errorMessage) {
}

// SyntaxAnalyzer 实现
SyntaxAnalyzer::SyntaxAnalyzer(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::shared_ptr<SyntaxAnalysisResult> SyntaxAnalyzer::analyze(const std::string& code) {
    auto result = std::make_shared<SyntaxAnalysisResult>(true);
    
    try {
        // 解析令牌
        auto tokens = parseTokens(code);
        result->setTokens(tokens);
        
        // 验证语法
        if (!validateSyntax(code)) {
            result = std::make_shared<SyntaxAnalysisResult>(false, "Invalid syntax");
            return result;
        }
        
        // 检测对象类型
        result->setMetadata("isObject", isObject(code));
        result->setMetadata("isFunction", isFunction(code));
        result->setMetadata("isArray", isArray(code));
        result->setMetadata("isCHTLJSFunction", isCHTLJSFunction(code));
        
    } catch (const std::exception& e) {
        result = std::make_shared<SyntaxAnalysisResult>(false, e.what());
    }
    
    return result;
}

std::shared_ptr<SyntaxAnalysisResult> SyntaxAnalyzer::analyzeFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return std::make_shared<SyntaxAnalysisResult>(false, "Cannot open file: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();
    
    return analyze(code);
}

bool SyntaxAnalyzer::isObject(const std::string& code) const {
    std::string trimmed = trim(code);
    return trimmed.starts_with("{") && trimmed.ends_with("}") && isBalanced(trimmed, '{', '}');
}

bool SyntaxAnalyzer::isFunction(const std::string& code) const {
    std::string trimmed = trim(code);
    return trimmed.starts_with("function") || trimmed.starts_with("(") || trimmed.starts_with("=>");
}

bool SyntaxAnalyzer::isArray(const std::string& code) const {
    std::string trimmed = trim(code);
    return trimmed.starts_with("[") && trimmed.ends_with("]") && isBalanced(trimmed, '[', ']');
}

bool SyntaxAnalyzer::isCHTLJSFunction(const std::string& code) const {
    std::string trimmed = trim(code);
    return trimmed.starts_with("vir") || trimmed.starts_with("listen") || 
           trimmed.starts_with("animate") || trimmed.starts_with("router");
}

bool SyntaxAnalyzer::validateSyntax(const std::string& code) const {
    // 检查括号平衡
    if (!isBalanced(code, '(', ')') || !isBalanced(code, '{', '}') || !isBalanced(code, '[', ']')) {
        return false;
    }
    
    // 检查字符串引号
    bool inString = false;
    char stringChar = 0;
    
    for (char c : code) {
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
        } else if (inString && c == stringChar) {
            inString = false;
            stringChar = 0;
        }
    }
    
    return !inString;
}

bool SyntaxAnalyzer::validateObjectSyntax(const std::string& code) const {
    if (!isObject(code)) {
        return false;
    }
    
    std::string content = extractContent(code, '{', '}');
    auto properties = parseObjectProperties(content);
    
    // 验证属性语法
    for (const auto& [key, value] : properties) {
        if (key.empty() || value.empty()) {
            return false;
        }
    }
    
    return true;
}

bool SyntaxAnalyzer::validateFunctionSyntax(const std::string& code) const {
    if (!isFunction(code)) {
        return false;
    }
    
    // 检查函数参数语法
    size_t parenPos = code.find('(');
    if (parenPos == std::string::npos) {
        return false;
    }
    
    size_t closeParenPos = code.find(')', parenPos);
    if (closeParenPos == std::string::npos) {
        return false;
    }
    
    std::string params = code.substr(parenPos + 1, closeParenPos - parenPos - 1);
    auto parameters = parseFunctionParameters(params);
    
    return true;
}

bool SyntaxAnalyzer::validateArraySyntax(const std::string& code) const {
    if (!isArray(code)) {
        return false;
    }
    
    std::string content = extractContent(code, '[', ']');
    auto elements = parseArrayElements(content);
    
    return true;
}

std::vector<std::string> SyntaxAnalyzer::parseTokens(const std::string& code) const {
    return tokenize(code);
}

std::unordered_map<std::string, std::string> SyntaxAnalyzer::parseObjectProperties(const std::string& code) const {
    std::unordered_map<std::string, std::string> properties;
    
    std::string content = extractContent(code, '{', '}');
    auto lines = splitByDelimiter(content, ',');
    
    for (const auto& line : lines) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        
        size_t colonPos = trimmed.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string key = trim(trimmed.substr(0, colonPos));
        std::string value = trim(trimmed.substr(colonPos + 1));
        
        // 去除引号
        if (key.starts_with("\"") && key.ends_with("\"")) {
            key = key.substr(1, key.length() - 2);
        }
        if (value.starts_with("\"") && value.ends_with("\"")) {
            value = value.substr(1, value.length() - 2);
        }
        
        properties[key] = value;
    }
    
    return properties;
}

std::vector<std::string> SyntaxAnalyzer::parseFunctionParameters(const std::string& code) const {
    std::vector<std::string> parameters;
    
    if (code.empty()) {
        return parameters;
    }
    
    auto elements = splitByDelimiter(code, ',');
    for (const auto& element : elements) {
        std::string trimmed = trim(element);
        if (!trimmed.empty()) {
            parameters.push_back(trimmed);
        }
    }
    
    return parameters;
}

std::vector<std::string> SyntaxAnalyzer::parseArrayElements(const std::string& code) const {
    std::vector<std::string> elements;
    
    if (code.empty()) {
        return elements;
    }
    
    auto tokens = splitByDelimiter(code, ',');
    for (const auto& token : tokens) {
        std::string trimmed = trim(token);
        if (!trimmed.empty()) {
            elements.push_back(trimmed);
        }
    }
    
    return elements;
}

std::vector<std::string> SyntaxAnalyzer::tokenize(const std::string& code) const {
    std::vector<std::string> tokens;
    std::string current;
    
    for (char c : code) {
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || 
                   c == ',' || c == ':' || c == ';' || c == '=') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

bool SyntaxAnalyzer::isBalanced(const std::string& code, char open, char close) const {
    int count = 0;
    for (char c : code) {
        if (c == open) {
            count++;
        } else if (c == close) {
            count--;
            if (count < 0) {
                return false;
            }
        }
    }
    return count == 0;
}

std::string SyntaxAnalyzer::extractContent(const std::string& code, char open, char close) const {
    size_t start = code.find(open);
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = code.rfind(close);
    if (end == std::string::npos || end <= start) {
        return "";
    }
    
    return code.substr(start + 1, end - start - 1);
}

std::vector<std::string> SyntaxAnalyzer::splitByDelimiter(const std::string& code, char delimiter) const {
    std::vector<std::string> result;
    std::stringstream ss(code);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(trim(item));
    }
    
    return result;
}

std::string SyntaxAnalyzer::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

// ParameterBinder 实现
ParameterBinder::ParameterBinder(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::shared_ptr<ParameterBindingResult> ParameterBinder::bind(const std::string& functionSignature, 
                                                             const std::vector<std::string>& arguments) {
    auto result = std::make_shared<ParameterBindingResult>(true);
    
    try {
        auto parameters = parseFunctionSignature(functionSignature);
        
        if (parameters.size() != arguments.size()) {
            result = std::make_shared<ParameterBindingResult>(false, "Parameter count mismatch");
            return result;
        }
        
        for (size_t i = 0; i < parameters.size(); ++i) {
            std::string paramName = extractParameterName(parameters[i]);
            result->setBoundParameter(paramName, arguments[i]);
        }
        
    } catch (const std::exception& e) {
        result = std::make_shared<ParameterBindingResult>(false, e.what());
    }
    
    return result;
}

std::shared_ptr<ParameterBindingResult> ParameterBinder::bindWithPlaceholders(const std::string& functionSignature, 
                                                                             const std::unordered_map<std::string, std::string>& placeholders) {
    auto result = std::make_shared<ParameterBindingResult>(true);
    
    try {
        auto parameters = parseFunctionSignature(functionSignature);
        
        for (const auto& parameter : parameters) {
            std::string paramName = extractParameterName(parameter);
            auto it = placeholders.find(paramName);
            
            if (it != placeholders.end()) {
                result->setBoundParameter(paramName, it->second);
            } else if (isOptionalParameter(parameter)) {
                result->setBoundParameter(paramName, "");
            } else {
                result->addUnboundParameter(paramName);
            }
        }
        
    } catch (const std::exception& e) {
        result = std::make_shared<ParameterBindingResult>(false, e.what());
    }
    
    return result;
}

std::string ParameterBinder::fillValue(const std::string& template_, const std::unordered_map<std::string, std::string>& values) const {
    std::string result = template_;
    
    for (const auto& [key, value] : values) {
        std::string placeholder = generatePlaceholder(key);
        result = replacePlaceholder(result, placeholder, value);
    }
    
    return result;
}

std::string ParameterBinder::transform(const std::string& template_, const std::unordered_map<std::string, std::string>& transformations) const {
    std::string result = template_;
    
    for (const auto& [key, value] : transformations) {
        std::string placeholder = generatePlaceholder(key);
        result = replacePlaceholder(result, placeholder, value);
    }
    
    return result;
}

std::vector<std::string> ParameterBinder::extractPlaceholders(const std::string& template_) const {
    std::vector<std::string> placeholders;
    std::regex placeholderRegex(R"(\$\{([^}]+)\})");
    std::sregex_iterator iter(template_.begin(), template_.end(), placeholderRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        placeholders.push_back(iter->str());
    }
    
    return placeholders;
}

std::string ParameterBinder::replacePlaceholder(const std::string& template_, const std::string& placeholder, const std::string& value) const {
    std::string result = template_;
    size_t pos = 0;
    
    while ((pos = result.find(placeholder, pos)) != std::string::npos) {
        result.replace(pos, placeholder.length(), value);
        pos += value.length();
    }
    
    return result;
}

std::string ParameterBinder::replaceAllPlaceholders(const std::string& template_, const std::unordered_map<std::string, std::string>& replacements) const {
    std::string result = template_;
    
    for (const auto& [placeholder, value] : replacements) {
        result = replacePlaceholder(result, placeholder, value);
    }
    
    return result;
}

bool ParameterBinder::validateParameters(const std::string& functionSignature, const std::vector<std::string>& arguments) const {
    auto parameters = parseFunctionSignature(functionSignature);
    return parameters.size() == arguments.size();
}

bool ParameterBinder::validatePlaceholders(const std::string& template_, const std::vector<std::string>& placeholders) const {
    auto extractedPlaceholders = extractPlaceholders(template_);
    return extractedPlaceholders.size() == placeholders.size();
}

std::vector<std::string> ParameterBinder::parseFunctionSignature(const std::string& signature) const {
    std::vector<std::string> parameters;
    
    size_t start = signature.find('(');
    if (start == std::string::npos) {
        return parameters;
    }
    
    size_t end = signature.find(')', start);
    if (end == std::string::npos) {
        return parameters;
    }
    
    std::string paramsStr = signature.substr(start + 1, end - start - 1);
    std::stringstream ss(paramsStr);
    std::string param;
    
    while (std::getline(ss, param, ',')) {
        std::string trimmed = param;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
        
        if (!trimmed.empty()) {
            parameters.push_back(trimmed);
        }
    }
    
    return parameters;
}

std::string ParameterBinder::extractParameterName(const std::string& parameter) const {
    std::string trimmed = parameter;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    // 移除类型声明
    size_t spacePos = trimmed.find(' ');
    if (spacePos != std::string::npos) {
        trimmed = trimmed.substr(0, spacePos);
    }
    
    return trimmed;
}

std::string ParameterBinder::extractParameterType(const std::string& parameter) const {
    std::string trimmed = parameter;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    size_t spacePos = trimmed.find(' ');
    if (spacePos != std::string::npos) {
        return trimmed.substr(spacePos + 1);
    }
    
    return "string";
}

bool ParameterBinder::isOptionalParameter(const std::string& parameter) const {
    return parameter.find('?') != std::string::npos || parameter.find('=') != std::string::npos;
}

std::string ParameterBinder::generatePlaceholder(const std::string& name) const {
    return "${" + name + "}";
}

// CJMODScanner 实现
CJMODScanner::CJMODScanner(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_scanMode("default"), m_scanPattern(".*") {
}

std::vector<std::string> CJMODScanner::scan(const std::string& code) {
    if (m_scanMode == "pattern") {
        return scanByPattern(code, m_scanPattern);
    } else {
        return scanByMode(code, m_scanMode);
    }
}

std::vector<std::string> CJMODScanner::scanFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Cannot open file: " + filePath);
        return {};
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();
    
    return scan(code);
}

std::vector<std::string> CJMODScanner::scanDirectory(const std::string& directoryPath) {
    std::vector<std::string> results;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                auto fileResults = scanFile(filePath);
                results.insert(results.end(), fileResults.begin(), fileResults.end());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        m_context->addError("Error scanning directory: " + std::string(e.what()));
    }
    
    return results;
}

std::vector<std::string> CJMODScanner::filterResults(const std::vector<std::string>& results, const std::string& filter) const {
    std::vector<std::string> filtered;
    
    for (const auto& result : results) {
        if (matchesPattern(result, filter)) {
            filtered.push_back(result);
        }
    }
    
    return filtered;
}

std::vector<std::string> CJMODScanner::sortResults(const std::vector<std::string>& results, const std::string& sortBy) const {
    std::vector<std::string> sorted = results;
    
    if (sortBy == "alphabetical") {
        std::sort(sorted.begin(), sorted.end());
    } else if (sortBy == "length") {
        std::sort(sorted.begin(), sorted.end(), [](const std::string& a, const std::string& b) {
            return a.length() < b.length();
        });
    }
    
    return sorted;
}

std::unordered_map<std::string, std::vector<std::string>> CJMODScanner::groupResults(const std::vector<std::string>& results) const {
    std::unordered_map<std::string, std::vector<std::string>> grouped;
    
    for (const auto& result : results) {
        std::string key = result.substr(0, 1); // 按首字母分组
        grouped[key].push_back(result);
    }
    
    return grouped;
}

std::vector<std::string> CJMODScanner::scanByPattern(const std::string& code, const std::string& pattern) const {
    std::vector<std::string> results;
    
    try {
        std::regex regexPattern(pattern);
        std::sregex_iterator iter(code.begin(), code.end(), regexPattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            results.push_back(iter->str());
        }
    } catch (const std::regex_error& e) {
        m_context->addError("Invalid regex pattern: " + std::string(e.what()));
    }
    
    return results;
}

std::vector<std::string> CJMODScanner::scanByMode(const std::string& code, const std::string& mode) const {
    if (mode == "double-pointer") {
        return performDoublePointerScan(code);
    } else if (mode == "pre-truncation") {
        return performPreTruncationScan(code);
    } else {
        return scanByPattern(code, ".*");
    }
}

bool CJMODScanner::matchesPattern(const std::string& text, const std::string& pattern) const {
    try {
        std::regex regexPattern(pattern);
        return std::regex_match(text, regexPattern);
    } catch (const std::regex_error&) {
        return false;
    }
}

std::string CJMODScanner::applyOptions(const std::string& text, const std::unordered_map<std::string, std::string>& options) const {
    std::string result = text;
    
    for (const auto& [key, value] : options) {
        if (key == "trim") {
            result.erase(0, result.find_first_not_of(" \t"));
            result.erase(result.find_last_not_of(" \t") + 1);
        } else if (key == "lowercase") {
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        } else if (key == "uppercase") {
            std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        }
    }
    
    return result;
}

} // namespace CHTL