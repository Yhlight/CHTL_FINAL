#include "Scanner/UnifiedScanner.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

UnifiedScanner::UnifiedScanner() : debug_mode_(false) {
    // 初始化占位符计数器
    placeholder_counters_[CodeType::CHTL] = 0;
    placeholder_counters_[CodeType::CHTL_JS] = 0;
    placeholder_counters_[CodeType::CSS] = 0;
    placeholder_counters_[CodeType::JS] = 0;
}

UnifiedScanner::~UnifiedScanner() = default;

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::scan(const std::string& code) {
    std::vector<CodeFragment> fragments;
    size_t pos = 0;
    
    if (debug_mode_) {
        std::cout << "[UnifiedScanner] Starting scan of " << code.length() << " characters" << std::endl;
    }
    
    while (pos < code.length()) {
        // 跳过空白字符
        while (pos < code.length() && std::isspace(code[pos])) {
            pos++;
        }
        
        if (pos >= code.length()) break;
        
        // 尝试识别不同类型的代码
        CodeType detected_type = CodeType::UNKNOWN;
        size_t end_pos = pos;
        
        // 检查CHTL语法
        if (code[pos] == '[' || (pos + 1 < code.length() && code.substr(pos, 2) == "//")) {
            detected_type = CodeType::CHTL;
            end_pos = findCHTLBoundary(code, pos);
        }
        // 检查CHTL JS语法
        else if (code[pos] == '{' && pos > 0 && code[pos-1] != ' ') {
            // 检查是否是CHTL JS函数语法
            size_t check_pos = pos;
            while (check_pos > 0 && std::isspace(code[check_pos-1])) {
                check_pos--;
            }
            if (check_pos > 0 && std::isalnum(code[check_pos-1])) {
                detected_type = CodeType::CHTL_JS;
                end_pos = findCHTLJSBoundary(code, pos);
            }
        }
        // 检查CSS语法
        else if (code[pos] == '{' && pos > 0) {
            // 检查前面是否有选择器
            size_t check_pos = pos - 1;
            while (check_pos > 0 && std::isspace(code[check_pos])) {
                check_pos--;
            }
            if (check_pos > 0 && (code[check_pos] == '}' || code[check_pos] == ';' || 
                                 code.substr(check_pos, 2) == "*/" || code.substr(check_pos, 2) == "//")) {
                detected_type = CodeType::CSS;
                end_pos = findCSSBoundary(code, pos);
            }
        }
        // 默认为JS
        else {
            detected_type = CodeType::JS;
            end_pos = findJSBoundary(code, pos);
        }
        
        if (detected_type != CodeType::UNKNOWN && end_pos > pos) {
            CodeFragment fragment;
            fragment.type = detected_type;
            fragment.content = code.substr(pos, end_pos - pos);
            fragment.start_pos = pos;
            fragment.end_pos = end_pos;
            fragment.placeholder = generatePlaceholder(detected_type, placeholder_counters_[detected_type]++);
            
            // 验证代码片段
            if (validateCodeFragment(fragment.content, detected_type)) {
                fragments.push_back(fragment);
                placeholder_map_[fragment.placeholder] = fragment;
                
                if (debug_mode_) {
                    std::cout << "[UnifiedScanner] Found " << static_cast<int>(detected_type) 
                              << " fragment at " << pos << "-" << end_pos << std::endl;
                }
            }
            
            pos = end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::string UnifiedScanner::restorePlaceholders(const std::string& code, 
                                               const std::vector<CodeFragment>& fragments) {
    std::string result = code;
    
    // 按位置倒序替换，避免位置偏移
    for (auto it = fragments.rbegin(); it != fragments.rend(); ++it) {
        const CodeFragment& fragment = *it;
        result.replace(fragment.start_pos, fragment.end_pos - fragment.start_pos, fragment.placeholder);
    }
    
    return result;
}

void UnifiedScanner::setConfiguration(const std::string& key, const std::string& value) {
    configurations_[key] = value;
}

void UnifiedScanner::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::scanCHTL(const std::string& code, size_t start_pos) {
    std::vector<CodeFragment> fragments;
    size_t pos = start_pos;
    
    while (pos < code.length()) {
        // 查找CHTL语法边界
        size_t end_pos = findCHTLBoundary(code, pos);
        if (end_pos > pos) {
            CodeFragment fragment;
            fragment.type = CodeType::CHTL;
            fragment.content = code.substr(pos, end_pos - pos);
            fragment.start_pos = pos;
            fragment.end_pos = end_pos;
            fragment.placeholder = generatePlaceholder(CodeType::CHTL, placeholder_counters_[CodeType::CHTL]++);
            
            if (validateCodeFragment(fragment.content, CodeType::CHTL)) {
                fragments.push_back(fragment);
            }
            
            pos = end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::scanCHTLJS(const std::string& code, size_t start_pos) {
    std::vector<CodeFragment> fragments;
    size_t pos = start_pos;
    
    while (pos < code.length()) {
        // 查找CHTL JS语法边界
        size_t end_pos = findCHTLJSBoundary(code, pos);
        if (end_pos > pos) {
            CodeFragment fragment;
            fragment.type = CodeType::CHTL_JS;
            fragment.content = code.substr(pos, end_pos - pos);
            fragment.start_pos = pos;
            fragment.end_pos = end_pos;
            fragment.placeholder = generatePlaceholder(CodeType::CHTL_JS, placeholder_counters_[CodeType::CHTL_JS]++);
            
            if (validateCodeFragment(fragment.content, CodeType::CHTL_JS)) {
                fragments.push_back(fragment);
            }
            
            pos = end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::scanCSS(const std::string& code, size_t start_pos) {
    std::vector<CodeFragment> fragments;
    size_t pos = start_pos;
    
    while (pos < code.length()) {
        // 查找CSS语法边界
        size_t end_pos = findCSSBoundary(code, pos);
        if (end_pos > pos) {
            CodeFragment fragment;
            fragment.type = CodeType::CSS;
            fragment.content = code.substr(pos, end_pos - pos);
            fragment.start_pos = pos;
            fragment.end_pos = end_pos;
            fragment.placeholder = generatePlaceholder(CodeType::CSS, placeholder_counters_[CodeType::CSS]++);
            
            if (validateCodeFragment(fragment.content, CodeType::CSS)) {
                fragments.push_back(fragment);
            }
            
            pos = end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::scanJS(const std::string& code, size_t start_pos) {
    std::vector<CodeFragment> fragments;
    size_t pos = start_pos;
    
    while (pos < code.length()) {
        // 查找JS语法边界
        size_t end_pos = findJSBoundary(code, pos);
        if (end_pos > pos) {
            CodeFragment fragment;
            fragment.type = CodeType::JS;
            fragment.content = code.substr(pos, end_pos - pos);
            fragment.start_pos = pos;
            fragment.end_pos = end_pos;
            fragment.placeholder = generatePlaceholder(CodeType::JS, placeholder_counters_[CodeType::JS]++);
            
            if (validateCodeFragment(fragment.content, CodeType::JS)) {
                fragments.push_back(fragment);
            }
            
            pos = end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

size_t UnifiedScanner::findSyntaxBoundary(const std::string& code, size_t start_pos, 
                                         const std::string& open, const std::string& close) {
    if (start_pos >= code.length()) return start_pos;
    
    size_t pos = start_pos;
    int brace_count = 0;
    bool in_string = false;
    char string_char = 0;
    
    while (pos < code.length()) {
        char c = code[pos];
        
        if (!in_string) {
            if (c == '"' || c == '\'') {
                in_string = true;
                string_char = c;
            } else if (code.substr(pos, open.length()) == open) {
                brace_count++;
                pos += open.length() - 1;
            } else if (code.substr(pos, close.length()) == close) {
                brace_count--;
                if (brace_count == 0) {
                    return pos + close.length();
                }
                pos += close.length() - 1;
            }
        } else {
            if (c == string_char && (pos == 0 || code[pos-1] != '\\')) {
                in_string = false;
            }
        }
        
        pos++;
    }
    
    return pos;
}

size_t UnifiedScanner::findCHTLBoundary(const std::string& code, size_t start_pos) {
    if (start_pos >= code.length()) return start_pos;
    
    // CHTL语法特征：以[开头或//开头的注释
    if (code[start_pos] == '[') {
        return findSyntaxBoundary(code, start_pos, "[", "]");
    } else if (start_pos + 1 < code.length() && code.substr(start_pos, 2) == "//") {
        // 单行注释，找到行尾
        size_t pos = start_pos;
        while (pos < code.length() && code[pos] != '\n') {
            pos++;
        }
        return pos;
    } else if (start_pos + 1 < code.length() && code.substr(start_pos, 2) == "/*") {
        // 多行注释，找到*/
        size_t pos = start_pos;
        while (pos + 1 < code.length()) {
            if (code.substr(pos, 2) == "*/") {
                return pos + 2;
            }
            pos++;
        }
        return pos;
    }
    
    return start_pos;
}

size_t UnifiedScanner::findCHTLJSBoundary(const std::string& code, size_t start_pos) {
    if (start_pos >= code.length()) return start_pos;
    
    // CHTL JS语法特征：{ } 块
    if (code[start_pos] == '{') {
        return findSyntaxBoundary(code, start_pos, "{", "}");
    }
    
    return start_pos;
}

size_t UnifiedScanner::findCSSBoundary(const std::string& code, size_t start_pos) {
    if (start_pos >= code.length()) return start_pos;
    
    // CSS语法特征：{ } 块
    if (code[start_pos] == '{') {
        return findSyntaxBoundary(code, start_pos, "{", "}");
    }
    
    return start_pos;
}

size_t UnifiedScanner::findJSBoundary(const std::string& code, size_t start_pos) {
    if (start_pos >= code.length()) return start_pos;
    
    // JS语法特征：函数、对象、语句等
    size_t pos = start_pos;
    
    // 跳过空白字符
    while (pos < code.length() && std::isspace(code[pos])) {
        pos++;
    }
    
    if (pos >= code.length()) return pos;
    
    // 检查是否是JS关键字或标识符
    if (std::isalpha(code[pos]) || code[pos] == '_' || code[pos] == '$') {
        // 找到标识符的结束位置
        while (pos < code.length() && (std::isalnum(code[pos]) || code[pos] == '_' || code[pos] == '$')) {
            pos++;
        }
        
        // 检查是否是函数调用
        while (pos < code.length() && std::isspace(code[pos])) {
            pos++;
        }
        
        if (pos < code.length() && code[pos] == '(') {
            return findSyntaxBoundary(code, pos, "(", ")");
        } else if (pos < code.length() && code[pos] == '{') {
            return findSyntaxBoundary(code, pos, "{", "}");
        }
    } else if (code[pos] == '{') {
        return findSyntaxBoundary(code, pos, "{", "}");
    } else if (code[pos] == '(') {
        return findSyntaxBoundary(code, pos, "(", ")");
    }
    
    // 默认到下一个分号或换行
    while (pos < code.length() && code[pos] != ';' && code[pos] != '\n') {
        pos++;
    }
    
    if (pos < code.length() && code[pos] == ';') {
        pos++;
    }
    
    return pos;
}

std::string UnifiedScanner::generatePlaceholder(CodeType type, size_t index) {
    std::string prefix;
    switch (type) {
        case CodeType::CHTL:
            prefix = "_CHTL_CODE_PLACEHOLDER_";
            break;
        case CodeType::CHTL_JS:
            prefix = "_CHTL_JS_CODE_PLACEHOLDER_";
            break;
        case CodeType::CSS:
            prefix = "_CSS_CODE_PLACEHOLDER_";
            break;
        case CodeType::JS:
            prefix = "_JS_CODE_PLACEHOLDER_";
            break;
        default:
            prefix = "_UNKNOWN_CODE_PLACEHOLDER_";
            break;
    }
    
    return prefix + std::to_string(index) + "_";
}

bool UnifiedScanner::validateCodeFragment(const std::string& code, CodeType type) {
    if (code.empty()) return false;
    
    switch (type) {
        case CodeType::CHTL:
            // 验证CHTL语法：检查括号匹配
            return validateCHTLSyntax(code);
        case CodeType::CHTL_JS:
            // 验证CHTL JS语法：检查大括号匹配
            return validateCHTLJSSyntax(code);
        case CodeType::CSS:
            // 验证CSS语法：检查大括号匹配
            return validateCSSSyntax(code);
        case CodeType::JS:
            // 验证JS语法：基本语法检查
            return validateJSSyntax(code);
        default:
            return false;
    }
}

std::string UnifiedScanner::processNestedStructures(const std::string& code, CodeType type) {
    // 处理嵌套结构，确保语法完整性
    std::string result = code;
    
    // 根据类型进行不同的处理
    switch (type) {
        case CodeType::CHTL:
        case CodeType::CHTL_JS:
        case CodeType::CSS:
            // 这些类型都使用大括号
            result = processBraceStructures(result);
            break;
        case CodeType::JS:
            // JS可能使用多种结构
            result = processJSStructures(result);
            break;
        default:
            break;
    }
    
    return result;
}

std::string UnifiedScanner::replaceWithPlaceholders(const std::string& code, 
                                                   const std::vector<CodeFragment>& fragments) {
    std::string result = code;
    
    // 按位置倒序替换，避免位置偏移
    for (auto it = fragments.rbegin(); it != fragments.rend(); ++it) {
        const CodeFragment& fragment = *it;
        result.replace(fragment.start_pos, fragment.end_pos - fragment.start_pos, fragment.placeholder);
    }
    
    return result;
}

bool UnifiedScanner::validateCHTLSyntax(const std::string& code) {
    int brace_count = 0;
    int bracket_count = 0;
    bool in_string = false;
    char string_char = 0;
    
    for (size_t i = 0; i < code.length(); i++) {
        char c = code[i];
        
        if (!in_string) {
            if (c == '"' || c == '\'') {
                in_string = true;
                string_char = c;
            } else if (c == '{') {
                brace_count++;
            } else if (c == '}') {
                brace_count--;
            } else if (c == '[') {
                bracket_count++;
            } else if (c == ']') {
                bracket_count--;
            }
        } else {
            if (c == string_char && (i == 0 || code[i-1] != '\\')) {
                in_string = false;
            }
        }
    }
    
    return brace_count == 0 && bracket_count == 0 && !in_string;
}

bool UnifiedScanner::validateCHTLJSSyntax(const std::string& code) {
    int brace_count = 0;
    bool in_string = false;
    char string_char = 0;
    
    for (size_t i = 0; i < code.length(); i++) {
        char c = code[i];
        
        if (!in_string) {
            if (c == '"' || c == '\'') {
                in_string = true;
                string_char = c;
            } else if (c == '{') {
                brace_count++;
            } else if (c == '}') {
                brace_count--;
            }
        } else {
            if (c == string_char && (i == 0 || code[i-1] != '\\')) {
                in_string = false;
            }
        }
    }
    
    return brace_count == 0 && !in_string;
}

bool UnifiedScanner::validateCSSSyntax(const std::string& code) {
    int brace_count = 0;
    bool in_string = false;
    char string_char = 0;
    
    for (size_t i = 0; i < code.length(); i++) {
        char c = code[i];
        
        if (!in_string) {
            if (c == '"' || c == '\'') {
                in_string = true;
                string_char = c;
            } else if (c == '{') {
                brace_count++;
            } else if (c == '}') {
                brace_count--;
            }
        } else {
            if (c == string_char && (i == 0 || code[i-1] != '\\')) {
                in_string = false;
            }
        }
    }
    
    return brace_count == 0 && !in_string;
}

bool UnifiedScanner::validateJSSyntax(const std::string& code) {
    // 基本的JS语法验证
    int brace_count = 0;
    int paren_count = 0;
    bool in_string = false;
    char string_char = 0;
    
    for (size_t i = 0; i < code.length(); i++) {
        char c = code[i];
        
        if (!in_string) {
            if (c == '"' || c == '\'') {
                in_string = true;
                string_char = c;
            } else if (c == '{') {
                brace_count++;
            } else if (c == '}') {
                brace_count--;
            } else if (c == '(') {
                paren_count++;
            } else if (c == ')') {
                paren_count--;
            }
        } else {
            if (c == string_char && (i == 0 || code[i-1] != '\\')) {
                in_string = false;
            }
        }
    }
    
    return brace_count == 0 && paren_count == 0 && !in_string;
}

std::string UnifiedScanner::processBraceStructures(const std::string& code) {
    // 处理大括号结构
    return code; // 简化实现
}

std::string UnifiedScanner::processJSStructures(const std::string& code) {
    // 处理JS结构
    return code; // 简化实现
}

} // namespace CHTL