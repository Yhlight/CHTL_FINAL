#include "Scanner/UnifiedScanner.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

UnifiedScanner::UnifiedScanner() : debug_mode_(false) {
    // 初始化占位符计数器
    placeholder_counters_[FragmentType::CHTL] = 0;
    placeholder_counters_[FragmentType::CHTL_JS] = 0;
    placeholder_counters_[FragmentType::CSS] = 0;
    placeholder_counters_[FragmentType::JAVASCRIPT] = 0;
}

UnifiedScanner::~UnifiedScanner() = default;

ScanResult UnifiedScanner::scan(const std::string& code) {
    ScanResult result;
    result.success = true;
    
    std::vector<CodeFragment> fragments;
    size_t pos = 0;
    
    while (pos < code.length()) {
        // 跳过空白字符
        while (pos < code.length() && std::isspace(code[pos])) {
            if (code[pos] == '\n') {
                line_++;
                column_ = 0;
            } else {
                column_++;
            }
            pos++;
        }
        
        if (pos >= code.length()) break;
        
        // 尝试识别不同类型的代码
        FragmentType detected_type = FragmentType::UNKNOWN;
        size_t end_pos = pos;
        
        // 检查CHTL语法
        if (code[pos] == '[' || (pos + 1 < code.length() && code.substr(pos, 2) == "//")) {
            detected_type = FragmentType::CHTL;
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
                detected_type = FragmentType::CHTL_JS;
                end_pos = findCHTLJSBoundary(code, pos);
            }
        }
        // 检查CSS语法
        else if (code[pos] == '{' && pos > 0) {
            // 检查前面是否有CSS选择器
            size_t check_pos = pos;
            while (check_pos > 0 && std::isspace(code[check_pos])) {
                check_pos--;
            }
            if (check_pos > 0 && (code[check_pos] == '}' || code[check_pos] == ';' || 
                                 code.substr(check_pos, 2) == "*/" || code.substr(check_pos, 2) == "//")) {
                detected_type = FragmentType::CSS;
                end_pos = findCSSBoundary(code, pos);
            }
        }
        // 默认为JS
        else {
            detected_type = FragmentType::JAVASCRIPT;
            end_pos = findJSBoundary(code, pos);
        }
        
        if (detected_type != FragmentType::UNKNOWN && end_pos > pos) {
            CodeFragment fragment;
            fragment.type = detected_type;
            fragment.content = code.substr(pos, end_pos - pos);
            fragment.position = pos;
            fragment.line = line_;
            fragment.column = column_;
            
            // 验证代码片段
            if (validateCodeFragment(fragment.content, detected_type)) {
                fragments.push_back(fragment);
                
                if (debug_mode_) {
                    std::cout << "[UnifiedScanner] Found " << static_cast<int>(detected_type) 
                              << " fragment at " << pos << "-" << end_pos << std::endl;
                }
            } else {
                result.errors.push_back("Invalid " + getFragmentTypeName(detected_type) + " fragment");
            }
            
            pos = end_pos;
        } else {
            // 如果无法识别，跳过当前字符
            pos++;
            column_++;
        }
    }
    
    result.fragments = fragments;
    return result;
}

size_t UnifiedScanner::findCHTLBoundary(const std::string& code, size_t start_pos) {
    // 简化的CHTL边界查找
    size_t pos = start_pos;
    
    // 查找匹配的 ']' 或 '}'
    if (code[pos] == '[') {
        int bracket_count = 1;
        pos++;
        while (pos < code.length() && bracket_count > 0) {
            if (code[pos] == '[') bracket_count++;
            else if (code[pos] == ']') bracket_count--;
            pos++;
        }
    } else if (code[pos] == '{') {
        int brace_count = 1;
        pos++;
        while (pos < code.length() && brace_count > 0) {
            if (code[pos] == '{') brace_count++;
            else if (code[pos] == '}') brace_count--;
            pos++;
        }
    }
    
    return pos;
}

size_t UnifiedScanner::findCHTLJSBoundary(const std::string& code, size_t start_pos) {
    // 简化的CHTL JS边界查找
    size_t pos = start_pos;
    
    if (code[pos] == '{') {
        int brace_count = 1;
        pos++;
        while (pos < code.length() && brace_count > 0) {
            if (code[pos] == '{') brace_count++;
            else if (code[pos] == '}') brace_count--;
            pos++;
        }
    }
    
    return pos;
}

size_t UnifiedScanner::findCSSBoundary(const std::string& code, size_t start_pos) {
    // 简化的CSS边界查找
    size_t pos = start_pos;
    
    if (code[pos] == '{') {
        int brace_count = 1;
        pos++;
        while (pos < code.length() && brace_count > 0) {
            if (code[pos] == '{') brace_count++;
            else if (code[pos] == '}') brace_count--;
            pos++;
        }
    }
    
    return pos;
}

size_t UnifiedScanner::findJSBoundary(const std::string& code, size_t start_pos) {
    // 简化的JS边界查找
    size_t pos = start_pos;
    
    // 查找下一个分号或换行
    while (pos < code.length() && code[pos] != ';' && code[pos] != '\n') {
        pos++;
    }
    
    if (pos < code.length()) {
        pos++; // 包含分号或换行
    }
    
    return pos;
}

bool UnifiedScanner::validateCodeFragment(const std::string& content, FragmentType type) {
    // 简化的代码片段验证
    switch (type) {
        case FragmentType::CHTL:
            return content.find('[') != std::string::npos || content.find("//") != std::string::npos;
        case FragmentType::CHTL_JS:
            return content.find('{') != std::string::npos;
        case FragmentType::CSS:
            return content.find('{') != std::string::npos && content.find('}') != std::string::npos;
        case FragmentType::JAVASCRIPT:
            return content.find(';') != std::string::npos || content.find('{') != std::string::npos;
        default:
            return true;
    }
}

std::string UnifiedScanner::getFragmentTypeName(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL: return "CHTL";
        case FragmentType::CHTL_JS: return "CHTL_JS";
        case FragmentType::CSS: return "CSS";
        case FragmentType::JAVASCRIPT: return "JavaScript";
        case FragmentType::HTML: return "HTML";
        case FragmentType::TEXT: return "Text";
        default: return "Unknown";
    }
}

void UnifiedScanner::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
}

bool UnifiedScanner::isDebugMode() const {
    return debug_mode_;
}

void UnifiedScanner::reportError(const std::string& message) {
    errors_.push_back(message);
}

void UnifiedScanner::clearErrors() {
    errors_.clear();
}

std::vector<std::string> UnifiedScanner::getErrors() const {
    return errors_;
}

bool UnifiedScanner::hasErrors() const {
    return !errors_.empty();
}

void UnifiedScanner::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 0;
    errors_.clear();
    placeholder_counters_.clear();
    placeholder_counters_[FragmentType::CHTL] = 0;
    placeholder_counters_[FragmentType::CHTL_JS] = 0;
    placeholder_counters_[FragmentType::CSS] = 0;
    placeholder_counters_[FragmentType::JAVASCRIPT] = 0;
}

std::string UnifiedScanner::getCurrentFilePath() const {
    return file_path_;
}

int UnifiedScanner::getCurrentLine() const {
    return line_;
}

int UnifiedScanner::getCurrentColumn() const {
    return column_;
}

int UnifiedScanner::getCurrentPosition() const {
    return position_;
}

std::map<std::string, std::vector<std::string>> UnifiedScanner::getBoundaries() const {
    return boundaries_;
}

void UnifiedScanner::setBoundaries(const std::map<std::string, std::vector<std::string>>& boundaries) {
    boundaries_ = boundaries;
}

} // namespace CHTL