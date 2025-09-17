#include "Syntax.h"
#include "Arg.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <cctype>

namespace CHTL {
namespace CJMOD {

// 静态常量定义
const std::vector<std::string> Syntax::PLACEHOLDER_PREFIXES = {"$", "$?", "$!", "$_", "$?_", "$!_"};
const std::vector<std::string> Syntax::OPERATOR_TOKENS = {"+", "-", "*", "/", "%", "**", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", "&", "|", "^", "~"};
const std::vector<std::string> Syntax::DELIMITER_TOKENS = {"(", ")", "{", "}", "[", "]", ",", ";", ":", ".", "?", "=>"};
const std::vector<std::string> Syntax::KEYWORD_TOKENS = {"function", "const", "let", "var", "if", "else", "for", "while", "do", "switch", "case", "default", "break", "continue", "return", "try", "catch", "finally", "throw", "new", "this", "super", "class", "extends", "implements", "interface", "enum", "public", "private", "protected", "static", "abstract", "final", "native", "synchronized", "volatile", "transient", "strictfp"};

std::shared_ptr<Arg> Syntax::analyze(const std::string& syntax) {
    if (!isValidSyntax(syntax)) {
        return nullptr;
    }
    
    auto arg = std::make_shared<Arg>();
    std::vector<std::string> tokens = tokenize(syntax);
    
    for (const auto& token : tokens) {
        arg->addToken(parseToken(token));
    }
    
    return arg;
}

bool Syntax::isObject(const std::string& code) {
    std::string trimmed = trimWhitespace(code);
    
    // 检查是否是对象字面量
    if (trimmed.empty() || trimmed.front() != '{' || trimmed.back() != '}') {
        return false;
    }
    
    // 移除外层大括号
    std::string content = trimmed.substr(1, trimmed.length() - 2);
    content = trimWhitespace(content);
    
    if (content.empty()) {
        return true; // 空对象 {}
    }
    
    // 检查对象内容格式
    return isBalanced(content);
}

bool Syntax::isFunction(const std::string& code) {
    std::string trimmed = trimWhitespace(code);
    
    // 检查函数声明
    if (std::regex_search(trimmed, std::regex(R"(function\s+\w+\s*\()"))) {
        return true;
    }
    
    // 检查箭头函数
    if (std::regex_search(trimmed, std::regex(R"(\s*\([^)]*\)\s*=>)"))) {
        return true;
    }
    
    // 检查函数表达式
    if (std::regex_search(trimmed, std::regex(R"(\w+\s*=\s*function\s*\()"))) {
        return true;
    }
    
    return false;
}

bool Syntax::isArray(const std::string& code) {
    std::string trimmed = trimWhitespace(code);
    
    // 检查是否是数组字面量
    if (trimmed.empty() || trimmed.front() != '[' || trimmed.back() != ']') {
        return false;
    }
    
    // 移除外层方括号
    std::string content = trimmed.substr(1, trimmed.length() - 2);
    content = trimWhitespace(content);
    
    if (content.empty()) {
        return true; // 空数组 []
    }
    
    // 检查数组内容格式
    return isBalanced(content);
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    std::string trimmed = trimWhitespace(code);
    
    // 检查CHTL JS函数格式: functionName { key: value, ... };
    std::regex chtljsFunctionRegex(R"(\w+\s*\{[^}]*\}\s*;?)");
    
    if (!std::regex_match(trimmed, chtljsFunctionRegex)) {
        return false;
    }
    
    // 查找函数名和参数块
    size_t bracePos = trimmed.find('{');
    if (bracePos == std::string::npos) {
        return false;
    }
    
    std::string functionName = trimWhitespace(trimmed.substr(0, bracePos));
    std::string params = trimmed.substr(bracePos);
    
    // 验证函数名
    if (!isIdentifier(functionName)) {
        return false;
    }
    
    // 验证参数块
    if (!isBalanced(params)) {
        return false;
    }
    
    return true;
}

bool Syntax::isValidSyntax(const std::string& syntax) {
    if (syntax.empty()) {
        return false;
    }
    
    // 检查括号平衡
    if (!isBalanced(syntax)) {
        return false;
    }
    
    // 检查占位符格式
    std::vector<std::string> placeholders = extractPlaceholders(syntax);
    for (const auto& placeholder : placeholders) {
        if (!isPlaceholderToken(placeholder)) {
            return false;
        }
    }
    
    return true;
}

bool Syntax::hasPlaceholder(const std::string& syntax) {
    for (const auto& prefix : PLACEHOLDER_PREFIXES) {
        if (syntax.find(prefix) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> Syntax::extractPlaceholders(const std::string& syntax) {
    std::vector<std::string> placeholders;
    std::regex placeholderRegex(R"(\$[!?_]*)");
    std::sregex_iterator iter(syntax.begin(), syntax.end(), placeholderRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        placeholders.push_back(iter->str());
    }
    
    return placeholders;
}

std::string Syntax::normalizeSyntax(const std::string& syntax) {
    std::string result = trimWhitespace(syntax);
    
    // 标准化空格
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 标准化操作符周围的空格
    for (const auto& op : OPERATOR_TOKENS) {
        std::string pattern = "\\s*" + std::regex_replace(op, std::regex(R"([\*\+\?\^\$\|\(\)\[\]\{\}\\])"), "\\$&") + "\\s*";
        std::string replacement = " " + op + " ";
        result = std::regex_replace(result, std::regex(pattern), replacement);
    }
    
    return trimWhitespace(result);
}

std::string Syntax::escapeSpecialChars(const std::string& syntax) {
    std::string result = syntax;
    
    // 转义特殊字符
    result = std::regex_replace(result, std::regex(R"(\\)"), "\\\\");
    result = std::regex_replace(result, std::regex(R"(\")"), "\\\"");
    result = std::regex_replace(result, std::regex(R"(\')"), "\\'");
    result = std::regex_replace(result, std::regex(R"(\n)"), "\\n");
    result = std::regex_replace(result, std::regex(R"(\r)"), "\\r");
    result = std::regex_replace(result, std::regex(R"(\t)"), "\\t");
    
    return result;
}

std::string Syntax::unescapeSpecialChars(const std::string& syntax) {
    std::string result = syntax;
    
    // 反转义特殊字符
    result = std::regex_replace(result, std::regex(R"(\\t)"), "\t");
    result = std::regex_replace(result, std::regex(R"(\\r)"), "\r");
    result = std::regex_replace(result, std::regex(R"(\\n)"), "\n");
    result = std::regex_replace(result, std::regex(R"(\\')"), "'");
    result = std::regex_replace(result, std::regex(R"(\\")"), "\"");
    result = std::regex_replace(result, std::regex(R"(\\\\)"), "\\");
    
    return result;
}

bool Syntax::matchPattern(const std::string& code, const std::string& pattern) {
    std::regex patternRegex(pattern);
    return std::regex_match(code, patternRegex);
}

std::vector<std::string> Syntax::extractMatches(const std::string& code, const std::string& pattern) {
    std::vector<std::string> matches;
    std::regex patternRegex(pattern);
    std::sregex_iterator iter(code.begin(), code.end(), patternRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        matches.push_back(iter->str());
    }
    
    return matches;
}

std::vector<std::string> Syntax::splitSyntax(const std::string& syntax) {
    return tokenize(syntax);
}

std::vector<std::string> Syntax::splitByPlaceholders(const std::string& syntax) {
    std::vector<std::string> parts;
    std::string current;
    
    for (size_t i = 0; i < syntax.length(); ++i) {
        char c = syntax[i];
        
        // 检查是否是占位符开始
        bool isPlaceholderStart = false;
        for (const auto& prefix : PLACEHOLDER_PREFIXES) {
            if (syntax.substr(i, prefix.length()) == prefix) {
                if (!current.empty()) {
                    parts.push_back(current);
                    current.clear();
                }
                parts.push_back(prefix);
                i += prefix.length() - 1; // 跳过占位符
                isPlaceholderStart = true;
                break;
            }
        }
        
        if (!isPlaceholderStart) {
            current += c;
        }
    }
    
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    return parts;
}

std::string Syntax::mergeSyntax(const std::vector<std::string>& parts) {
    std::ostringstream oss;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            oss << " ";
        }
        oss << parts[i];
    }
    return oss.str();
}

std::string Syntax::mergeWithPlaceholders(const std::vector<std::string>& parts, 
                                        const std::vector<std::string>& placeholders) {
    std::ostringstream oss;
    size_t placeholderIndex = 0;
    
    for (const auto& part : parts) {
        if (isPlaceholderToken(part)) {
            if (placeholderIndex < placeholders.size()) {
                oss << placeholders[placeholderIndex++];
            } else {
                oss << part;
            }
        } else {
            oss << part;
        }
    }
    
    return oss.str();
}

bool Syntax::isBalanced(const std::string& code) {
    int parenCount = 0;
    int braceCount = 0;
    int bracketCount = 0;
    bool inString = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < code.length(); ++i) {
        char c = code[i];
        
        if (inString) {
            if (c == stringChar && (i == 0 || code[i-1] != '\\')) {
                inString = false;
                stringChar = '\0';
            }
            continue;
        }
        
        if (c == '"' || c == '\'') {
            inString = true;
            stringChar = c;
            continue;
        }
        
        switch (c) {
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
        
        if (parenCount < 0 || braceCount < 0 || bracketCount < 0) {
            return false;
        }
    }
    
    return parenCount == 0 && braceCount == 0 && bracketCount == 0;
}

bool Syntax::isQuoted(const std::string& text) {
    if (text.length() < 2) {
        return false;
    }
    
    return (text.front() == '"' && text.back() == '"') ||
           (text.front() == '\'' && text.back() == '\'');
}

bool Syntax::isNumeric(const std::string& text) {
    if (text.empty()) {
        return false;
    }
    
    std::regex numberRegex(R"(-?\d+(\.\d+)?([eE][+-]?\d+)?)");
    return std::regex_match(text, numberRegex);
}

bool Syntax::isIdentifier(const std::string& text) {
    if (text.empty()) {
        return false;
    }
    
    // 第一个字符必须是字母、下划线或美元符号
    if (!std::isalpha(text[0]) && text[0] != '_' && text[0] != '$') {
        return false;
    }
    
    // 后续字符必须是字母、数字、下划线或美元符号
    for (size_t i = 1; i < text.length(); ++i) {
        if (!std::isalnum(text[i]) && text[i] != '_' && text[i] != '$') {
            return false;
        }
    }
    
    return true;
}

std::string Syntax::trimWhitespace(const std::string& text) {
    size_t start = text.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = text.find_last_not_of(" \t\n\r");
    return text.substr(start, end - start + 1);
}

std::vector<std::string> Syntax::tokenize(const std::string& syntax) {
    std::vector<std::string> tokens;
    std::string current;
    
    for (size_t i = 0; i < syntax.length(); ++i) {
        char c = syntax[i];
        
        // 检查是否是占位符
        bool isPlaceholder = false;
        for (const auto& prefix : PLACEHOLDER_PREFIXES) {
            if (syntax.substr(i, prefix.length()) == prefix) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
                tokens.push_back(prefix);
                i += prefix.length() - 1;
                isPlaceholder = true;
                break;
            }
        }
        
        if (isPlaceholder) {
            continue;
        }
        
        // 检查是否是操作符
        bool isOperator = false;
        for (const auto& op : OPERATOR_TOKENS) {
            if (syntax.substr(i, op.length()) == op) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
                tokens.push_back(op);
                i += op.length() - 1;
                isOperator = true;
                break;
            }
        }
        
        if (isOperator) {
            continue;
        }
        
        // 检查是否是分隔符
        bool isDelimiter = false;
        for (const auto& delim : DELIMITER_TOKENS) {
            if (syntax.substr(i, delim.length()) == delim) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
                tokens.push_back(delim);
                i += delim.length() - 1;
                isDelimiter = true;
                break;
            }
        }
        
        if (isDelimiter) {
            continue;
        }
        
        // 普通字符
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

std::string Syntax::parseToken(const std::string& token) {
    std::string trimmed = trimWhitespace(token);
    
    if (isPlaceholderToken(trimmed)) {
        return trimmed;
    }
    
    if (isOperatorToken(trimmed)) {
        return trimmed;
    }
    
    if (isDelimiterToken(trimmed)) {
        return trimmed;
    }
    
    if (isIdentifier(trimmed)) {
        return trimmed;
    }
    
    if (isNumeric(trimmed)) {
        return trimmed;
    }
    
    if (isQuoted(trimmed)) {
        return trimmed;
    }
    
    return trimmed;
}

bool Syntax::isPlaceholderToken(const std::string& token) {
    for (const auto& prefix : PLACEHOLDER_PREFIXES) {
        if (token == prefix) {
            return true;
        }
    }
    return false;
}

bool Syntax::isOperatorToken(const std::string& token) {
    return std::find(OPERATOR_TOKENS.begin(), OPERATOR_TOKENS.end(), token) != OPERATOR_TOKENS.end();
}

bool Syntax::isDelimiterToken(const std::string& token) {
    return std::find(DELIMITER_TOKENS.begin(), DELIMITER_TOKENS.end(), token) != DELIMITER_TOKENS.end();
}

} // namespace CJMOD
} // namespace CHTL