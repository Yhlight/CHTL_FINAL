#include "CJMOD/Syntax.h"
#include <iostream>
#include <cctype>
#include <algorithm>

namespace CJMOD {

std::vector<std::string> Syntax::analyze(const std::string& syntax) {
    std::vector<std::string> result;
    
    if (syntax.empty()) {
        return result;
    }
    
    size_t pos = 0;
    std::string current;
    
    while (pos < syntax.length()) {
        skipWhitespace(syntax, pos);
        
        if (pos >= syntax.length()) {
            break;
        }
        
        // 检查是否为占位符
        if (isPlaceholder(syntax, pos)) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
            result.push_back(extractPlaceholder(syntax, pos));
        } else {
            current += syntax[pos];
            pos++;
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
    }
    
    return result;
}

bool Syntax::isObject(const std::string& code) {
    if (code.empty()) {
        return false;
    }
    
    size_t pos = 0;
    skipWhitespace(code, pos);
    
    if (pos >= code.length() || code[pos] != '{') {
        return false;
    }
    
    // 检查括号匹配
    return checkBracketMatch(code);
}

bool Syntax::isFunction(const std::string& code) {
    if (code.empty()) {
        return false;
    }
    
    size_t pos = 0;
    skipWhitespace(code, pos);
    
    // 检查 function 关键字
    if (pos + 8 < code.length() && code.substr(pos, 8) == "function") {
        return true;
    }
    
    // 检查箭头函数
    if (pos < code.length() && code[pos] == '(') {
        size_t parenPos = code.find(')', pos);
        if (parenPos != std::string::npos && 
            parenPos + 2 < code.length() && 
            code.substr(parenPos + 1, 2) == "=>") {
            return true;
        }
    }
    
    return false;
}

bool Syntax::isArray(const std::string& code) {
    if (code.empty()) {
        return false;
    }
    
    size_t pos = 0;
    skipWhitespace(code, pos);
    
    if (pos >= code.length() || code[pos] != '[') {
        return false;
    }
    
    // 检查括号匹配
    return checkBracketMatch(code);
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    if (code.empty()) {
        return false;
    }
    
    size_t pos = 0;
    skipWhitespace(code, pos);
    
    // 检查 CHTL JS 函数特征
    // 1. 以标识符开头
    // 2. 后面跟着 { }
    // 3. 内部包含键值对语法
    
    std::string identifier = extractIdentifier(code, pos);
    if (identifier.empty()) {
        return false;
    }
    
    skipWhitespace(code, pos);
    
    if (pos >= code.length() || code[pos] != '{') {
        return false;
    }
    
    // 检查内部是否包含键值对语法
    size_t braceStart = pos;
    size_t braceEnd = findMatchingBrace(code, pos);
    
    if (braceEnd == std::string::npos) {
        return false;
    }
    
    std::string content = code.substr(braceStart + 1, braceEnd - braceStart - 1);
    return containsKeyValuePairs(content);
}

bool Syntax::validateSyntax(const std::string& syntax) {
    if (syntax.empty()) {
        return false;
    }
    
    // 检查括号匹配
    if (!checkBracketMatch(syntax)) {
        return false;
    }
    
    // 检查语法完整性
    return isComplete(syntax);
}

std::vector<std::string> Syntax::extractPlaceholders(const std::string& syntax) {
    std::vector<std::string> placeholders;
    
    size_t pos = 0;
    while (pos < syntax.length()) {
        if (isPlaceholder(syntax, pos)) {
            placeholders.push_back(extractPlaceholder(syntax, pos));
        } else {
            pos++;
        }
    }
    
    return placeholders;
}

bool Syntax::isComplete(const std::string& syntax) {
    if (syntax.empty()) {
        return false;
    }
    
    // 检查是否包含必要的语法元素
    bool hasPlaceholder = false;
    bool hasContent = false;
    
    size_t pos = 0;
    while (pos < syntax.length()) {
        if (isPlaceholder(syntax, pos)) {
            hasPlaceholder = true;
            extractPlaceholder(syntax, pos);
        } else if (!std::isspace(syntax[pos])) {
            hasContent = true;
        }
        pos++;
    }
    
    return hasPlaceholder || hasContent;
}

void Syntax::skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.length() && std::isspace(str[pos])) {
        pos++;
    }
}

bool Syntax::skipComment(const std::string& str, size_t& pos) {
    if (pos >= str.length()) {
        return false;
    }
    
    // 检查单行注释 //
    if (pos + 1 < str.length() && str[pos] == '/' && str[pos + 1] == '/') {
        while (pos < str.length() && str[pos] != '\n') {
            pos++;
        }
        return true;
    }
    
    // 检查多行注释 /* */
    if (pos + 1 < str.length() && str[pos] == '/' && str[pos + 1] == '*') {
        pos += 2;
        while (pos + 1 < str.length()) {
            if (str[pos] == '*' && str[pos + 1] == '/') {
                pos += 2;
                return true;
            }
            pos++;
        }
        return true;
    }
    
    return false;
}

bool Syntax::skipString(const std::string& str, size_t& pos) {
    if (pos >= str.length()) {
        return false;
    }
    
    char quote = str[pos];
    if (quote != '"' && quote != '\'') {
        return false;
    }
    
    pos++; // 跳过开始引号
    
    while (pos < str.length()) {
        if (str[pos] == quote) {
            pos++; // 跳过结束引号
            return true;
        }
        
        // 处理转义字符
        if (str[pos] == '\\' && pos + 1 < str.length()) {
            pos += 2;
        } else {
            pos++;
        }
    }
    
    return true; // 即使没有找到结束引号也返回true
}

bool Syntax::checkBracketMatch(const std::string& str) {
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        
        switch (c) {
            case '{':
                braceCount++;
                break;
            case '}':
                braceCount--;
                break;
            case '(':
                parenCount++;
                break;
            case ')':
                parenCount--;
                break;
            case '[':
                bracketCount++;
                break;
            case ']':
                bracketCount--;
                break;
        }
        
        if (braceCount < 0 || parenCount < 0 || bracketCount < 0) {
            return false;
        }
    }
    
    return braceCount == 0 && parenCount == 0 && bracketCount == 0;
}

std::string Syntax::extractIdentifier(const std::string& str, size_t& pos) {
    std::string identifier;
    
    while (pos < str.length() && 
           (std::isalnum(str[pos]) || str[pos] == '_' || str[pos] == '$')) {
        identifier += str[pos];
        pos++;
    }
    
    return identifier;
}

bool Syntax::isPlaceholder(const std::string& str, size_t pos) {
    if (pos >= str.length()) {
        return false;
    }
    
    // 检查 $ 占位符
    if (str[pos] == '$') {
        return true;
    }
    
    // 检查其他占位符模式
    return false;
}

std::string Syntax::extractPlaceholder(const std::string& str, size_t& pos) {
    std::string placeholder;
    
    if (pos >= str.length()) {
        return placeholder;
    }
    
    if (str[pos] == '$') {
        placeholder += str[pos];
        pos++;
        
        // 检查修饰符
        while (pos < str.length() && 
               (str[pos] == '?' || str[pos] == '!' || str[pos] == '_')) {
            placeholder += str[pos];
            pos++;
        }
    }
    
    return placeholder;
}

// 辅助函数实现
size_t Syntax::findMatchingBrace(const std::string& str, size_t startPos) {
    int count = 0;
    for (size_t i = startPos; i < str.length(); i++) {
        if (str[i] == '{') {
            count++;
        } else if (str[i] == '}') {
            count--;
            if (count == 0) {
                return i;
            }
        }
    }
    return std::string::npos;
}

bool Syntax::containsKeyValuePairs(const std::string& content) {
    // 简单的键值对检测
    // 检查是否包含 : 符号
    return content.find(':') != std::string::npos;
}

} // namespace CJMOD