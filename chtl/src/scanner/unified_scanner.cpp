#include "scanner/unified_scanner.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace chtl {
namespace scanner {

UnifiedScanner::UnifiedScanner(const std::string& source) 
    : source_(source), position_(0), line_(1), column_(1), placeholder_counter_(0),
      in_style_block_(false), in_script_block_(false), in_chtl_js_block_(false) {
    initialize_boundaries();
}

void UnifiedScanner::initialize_boundaries() {
    initialize_chtl_boundaries();
    initialize_js_boundaries();
    initialize_css_boundaries();
}

void UnifiedScanner::initialize_chtl_boundaries() {
    // CHTL语法边界
    boundaries_.emplace_back("style", "}", FragmentType::CSS, true);
    boundaries_.emplace_back("script", "}", FragmentType::CHTL_JS, true);
    boundaries_.emplace_back("text", "}", FragmentType::CHTL, false);
    boundaries_.emplace_back("[Template]", "}", FragmentType::CHTL, true);
    boundaries_.emplace_back("[Custom]", "}", FragmentType::CHTL, true);
    boundaries_.emplace_back("[Origin]", "}", FragmentType::CHTL, true);
    boundaries_.emplace_back("[Import]", "}", FragmentType::CHTL, true);
    boundaries_.emplace_back("[Namespace]", "}", FragmentType::CHTL, true);
    boundaries_.emplace_back("[Configuration]", "}", FragmentType::CHTL, true);
}

void UnifiedScanner::initialize_js_boundaries() {
    // JavaScript语法边界
    boundaries_.emplace_back("function", "}", FragmentType::JS, true);
    boundaries_.emplace_back("if", "}", FragmentType::JS, true);
    boundaries_.emplace_back("for", "}", FragmentType::JS, true);
    boundaries_.emplace_back("while", "}", FragmentType::JS, true);
    boundaries_.emplace_back("switch", "}", FragmentType::JS, true);
    boundaries_.emplace_back("try", "}", FragmentType::JS, true);
    boundaries_.emplace_back("class", "}", FragmentType::JS, true);
    boundaries_.emplace_back("const", ";", FragmentType::JS, false);
    boundaries_.emplace_back("let", ";", FragmentType::JS, false);
    boundaries_.emplace_back("var", ";", FragmentType::JS, false);
}

void UnifiedScanner::initialize_css_boundaries() {
    // CSS语法边界
    boundaries_.emplace_back("{", "}", FragmentType::CSS, true);
    boundaries_.emplace_back("@media", "}", FragmentType::CSS, true);
    boundaries_.emplace_back("@keyframes", "}", FragmentType::CSS, true);
    boundaries_.emplace_back("@import", ";", FragmentType::CSS, false);
    boundaries_.emplace_back("@charset", ";", FragmentType::CSS, false);
}

std::vector<CodeFragment> UnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    position_ = 0;
    line_ = 1;
    column_ = 1;
    context_stack_.clear();
    in_style_block_ = false;
    in_script_block_ = false;
    in_chtl_js_block_ = false;
    
    while (position_ < source_.length()) {
        skip_whitespace();
        
        if (position_ >= source_.length()) {
            break;
        }
        
        // 检查当前上下文
        if (is_style_boundary(position_)) {
            auto fragment = scan_style_fragment();
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
            }
        } else if (is_script_boundary(position_)) {
            auto fragment = scan_script_fragment();
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
            }
        } else if (is_chtl_js_boundary(position_)) {
            auto fragment = scan_chtl_js_fragment();
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
            }
        } else {
            // 默认作为CHTL处理
            auto fragment = scan_chtl_fragment();
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
            }
        }
    }
    
    return fragments;
}

CodeFragment UnifiedScanner::scan_chtl_fragment() {
    size_t start_pos = position_;
    int start_line = line_;
    int start_column = column_;
    
    // 扫描到下一个边界或文件结束
    while (position_ < source_.length()) {
        if (is_style_boundary(position_) || is_script_boundary(position_) || 
            is_chtl_js_boundary(position_)) {
            break;
        }
        advance();
    }
    
    std::string content = source_.substr(start_pos, position_ - start_pos);
    return CodeFragment(FragmentType::CHTL, content, start_pos, position_, start_line, start_column);
}

CodeFragment UnifiedScanner::scan_style_fragment() {
    size_t start_pos = position_;
    int start_line = line_;
    int start_column = column_;
    
    // 跳过 "style" 关键字
    while (position_ < source_.length() && !is_whitespace(current_char()) && current_char() != '{') {
        advance();
    }
    
    // 跳过空白字符
    skip_whitespace();
    
    // 找到开始的大括号
    if (current_char() == '{') {
        advance();
        
        // 扫描CSS内容直到匹配的结束大括号
        int brace_count = 1;
        while (position_ < source_.length() && brace_count > 0) {
            if (current_char() == '{') {
                brace_count++;
            } else if (current_char() == '}') {
                brace_count--;
            }
            advance();
        }
    }
    
    std::string content = source_.substr(start_pos, position_ - start_pos);
    return CodeFragment(FragmentType::CSS, content, start_pos, position_, start_line, start_column);
}

CodeFragment UnifiedScanner::scan_script_fragment() {
    size_t start_pos = position_;
    int start_line = line_;
    int start_column = column_;
    
    // 跳过 "script" 关键字
    while (position_ < source_.length() && !is_whitespace(current_char()) && current_char() != '{') {
        advance();
    }
    
    // 跳过空白字符
    skip_whitespace();
    
    // 找到开始的大括号
    if (current_char() == '{') {
        advance();
        
        // 扫描脚本内容直到匹配的结束大括号
        int brace_count = 1;
        while (position_ < source_.length() && brace_count > 0) {
            if (current_char() == '{') {
                brace_count++;
            } else if (current_char() == '}') {
                brace_count--;
            }
            advance();
        }
    }
    
    std::string content = source_.substr(start_pos, position_ - start_pos);
    
    // 判断是CHTL JS还是纯JS
    FragmentType type = wide_judgment(content, FragmentType::CHTL_JS) ? 
                       FragmentType::CHTL_JS : FragmentType::JS;
    
    return CodeFragment(type, content, start_pos, position_, start_line, start_column);
}

CodeFragment UnifiedScanner::scan_chtl_js_fragment() {
    size_t start_pos = position_;
    int start_line = line_;
    int start_column = column_;
    
    // 扫描CHTL JS特定语法
    while (position_ < source_.length()) {
        // 检查是否是CHTL JS关键字
        if (match_pattern("fileloader", position_) ||
            match_pattern("listen", position_) ||
            match_pattern("delegate", position_) ||
            match_pattern("animate", position_) ||
            match_pattern("vir", position_) ||
            match_pattern("router", position_)) {
            // 扫描到下一个分号或大括号
            while (position_ < source_.length() && current_char() != ';' && current_char() != '{') {
                advance();
            }
            if (current_char() == '{') {
                int brace_count = 1;
                advance();
                while (position_ < source_.length() && brace_count > 0) {
                    if (current_char() == '{') {
                        brace_count++;
                    } else if (current_char() == '}') {
                        brace_count--;
                    }
                    advance();
                }
            } else if (current_char() == ';') {
                advance();
            }
        } else {
            advance();
        }
    }
    
    std::string content = source_.substr(start_pos, position_ - start_pos);
    return CodeFragment(FragmentType::CHTL_JS, content, start_pos, position_, start_line, start_column);
}

CodeFragment UnifiedScanner::scan_js_fragment() {
    size_t start_pos = position_;
    int start_line = line_;
    int start_column = column_;
    
    // 扫描纯JavaScript代码
    while (position_ < source_.length()) {
        advance();
    }
    
    std::string content = source_.substr(start_pos, position_ - start_pos);
    return CodeFragment(FragmentType::JS, content, start_pos, position_, start_line, start_column);
}

bool UnifiedScanner::is_style_boundary(size_t pos) {
    return match_pattern("style", pos);
}

bool UnifiedScanner::is_script_boundary(size_t pos) {
    return match_pattern("script", pos);
}

bool UnifiedScanner::is_chtl_js_boundary(size_t pos) {
    return match_pattern("fileloader", pos) ||
           match_pattern("listen", pos) ||
           match_pattern("delegate", pos) ||
           match_pattern("animate", pos) ||
           match_pattern("vir", pos) ||
           match_pattern("router", pos);
}

bool UnifiedScanner::is_js_boundary(size_t pos) {
    return match_pattern("function", pos) ||
           match_pattern("const", pos) ||
           match_pattern("let", pos) ||
           match_pattern("var", pos) ||
           match_pattern("if", pos) ||
           match_pattern("for", pos) ||
           match_pattern("while", pos);
}

std::string UnifiedScanner::create_placeholder() {
    std::string placeholder = "_CHTL_PLACEHOLDER_" + std::to_string(placeholder_counter_++) + "_";
    return placeholder;
}

void UnifiedScanner::replace_with_placeholder(const CodeFragment& fragment) {
    std::string placeholder = create_placeholder();
    PlaceholderInfo info(placeholder, fragment.type, fragment.content, 
                        fragment.start_pos, fragment.end_pos, fragment.line, fragment.column);
    placeholders_.push_back(info);
    placeholder_map_[placeholder] = info;
}

std::string UnifiedScanner::restore_placeholders(const std::string& processed_content) {
    std::string result = processed_content;
    
    // 按位置倒序替换，避免位置偏移
    std::sort(placeholders_.begin(), placeholders_.end(), 
              [](const PlaceholderInfo& a, const PlaceholderInfo& b) {
                  return a.start_pos > b.start_pos;
              });
    
    for (const auto& placeholder : placeholders_) {
        size_t pos = result.find(placeholder.placeholder);
        if (pos != std::string::npos) {
            result.replace(pos, placeholder.placeholder.length(), placeholder.original_content);
        }
    }
    
    return result;
}

bool UnifiedScanner::wide_judgment(const std::string& content, FragmentType type) {
    // 宽判：检查是否包含CHTL JS特定语法
    if (type == FragmentType::CHTL_JS) {
        return content.find("fileloader") != std::string::npos ||
               content.find("listen") != std::string::npos ||
               content.find("delegate") != std::string::npos ||
               content.find("animate") != std::string::npos ||
               content.find("vir") != std::string::npos ||
               content.find("router") != std::string::npos ||
               content.find("{{") != std::string::npos;  // CHTL JS选择器语法
    }
    return false;
}

bool UnifiedScanner::strict_judgment(const std::string& content, FragmentType type) {
    // 严判：更严格的语法检查
    if (type == FragmentType::CHTL_JS) {
        // 检查CHTL JS特定的语法模式
        return content.find("fileloader {") != std::string::npos ||
               content.find("listen {") != std::string::npos ||
               content.find("delegate {") != std::string::npos ||
               content.find("animate {") != std::string::npos ||
               content.find("vir {") != std::string::npos ||
               content.find("router {") != std::string::npos;
    }
    return false;
}

std::string UnifiedScanner::process_for_chtl_compiler() {
    auto fragments = scan();
    std::string result = source_;
    
    // 将非CHTL片段替换为占位符
    for (const auto& fragment : fragments) {
        if (fragment.type != FragmentType::CHTL) {
            replace_with_placeholder(fragment);
            std::string placeholder = placeholders_.back().placeholder;
            result.replace(fragment.start_pos, fragment.end_pos - fragment.start_pos, placeholder);
        }
    }
    
    return result;
}

std::string UnifiedScanner::process_for_chtl_js_compiler() {
    auto fragments = scan();
    std::string result = source_;
    
    // 将非CHTL JS片段替换为占位符
    for (const auto& fragment : fragments) {
        if (fragment.type != FragmentType::CHTL_JS) {
            replace_with_placeholder(fragment);
            std::string placeholder = placeholders_.back().placeholder;
            result.replace(fragment.start_pos, fragment.end_pos - fragment.start_pos, placeholder);
        }
    }
    
    return result;
}

std::string UnifiedScanner::process_for_js_compiler() {
    auto fragments = scan();
    std::string result = source_;
    
    // 将非JS片段替换为占位符
    for (const auto& fragment : fragments) {
        if (fragment.type != FragmentType::JS) {
            replace_with_placeholder(fragment);
            std::string placeholder = placeholders_.back().placeholder;
            result.replace(fragment.start_pos, fragment.end_pos - fragment.start_pos, placeholder);
        }
    }
    
    return result;
}

std::string UnifiedScanner::process_for_css_compiler() {
    auto fragments = scan();
    std::string result = source_;
    
    // 将非CSS片段替换为占位符
    for (const auto& fragment : fragments) {
        if (fragment.type != FragmentType::CSS) {
            replace_with_placeholder(fragment);
            std::string placeholder = placeholders_.back().placeholder;
            result.replace(fragment.start_pos, fragment.end_pos - fragment.start_pos, placeholder);
        }
    }
    
    return result;
}

void UnifiedScanner::print_fragments() const {
    auto fragments = const_cast<UnifiedScanner*>(this)->scan();
    
    std::cout << "=== Code Fragments ===" << std::endl;
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        std::cout << "Fragment " << i << ":" << std::endl;
        std::cout << "  Type: " << static_cast<int>(fragment.type) << std::endl;
        std::cout << "  Content: " << fragment.content.substr(0, 100) << (fragment.content.length() > 100 ? "..." : "") << std::endl;
        std::cout << "  Position: " << fragment.start_pos << "-" << fragment.end_pos << std::endl;
        std::cout << "  Line: " << fragment.line << ", Column: " << fragment.column << std::endl;
        std::cout << std::endl;
    }
}

void UnifiedScanner::print_placeholders() const {
    std::cout << "=== Placeholders ===" << std::endl;
    for (const auto& placeholder : placeholders_) {
        std::cout << "Placeholder: " << placeholder.placeholder << std::endl;
        std::cout << "  Type: " << static_cast<int>(placeholder.original_type) << std::endl;
        std::cout << "  Content: " << placeholder.original_content.substr(0, 50) << "..." << std::endl;
        std::cout << "  Position: " << placeholder.start_pos << "-" << placeholder.end_pos << std::endl;
        std::cout << std::endl;
    }
}

std::string UnifiedScanner::get_processed_content() {
    return restore_placeholders(source_);
}

void UnifiedScanner::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
    context_stack_.clear();
    in_style_block_ = false;
    in_script_block_ = false;
    in_chtl_js_block_ = false;
}

void UnifiedScanner::clear_placeholders() {
    placeholders_.clear();
    placeholder_map_.clear();
    placeholder_counter_ = 0;
}

// 工具方法实现
char UnifiedScanner::current_char() const {
    if (position_ >= source_.length()) {
        return '\0';
    }
    return source_[position_];
}

char UnifiedScanner::peek_char(size_t offset) const {
    size_t index = position_ + offset;
    if (index >= source_.length()) {
        return '\0';
    }
    return source_[index];
}

void UnifiedScanner::advance() {
    if (position_ < source_.length()) {
        if (source_[position_] == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        position_++;
    }
}

void UnifiedScanner::skip_whitespace() {
    while (position_ < source_.length() && is_whitespace(current_char())) {
        advance();
    }
}

bool UnifiedScanner::is_whitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool UnifiedScanner::is_alpha(char c) const {
    return std::isalpha(c);
}

bool UnifiedScanner::is_digit(char c) const {
    return std::isdigit(c);
}

bool UnifiedScanner::is_alnum(char c) const {
    return std::isalnum(c);
}

bool UnifiedScanner::match_pattern(const std::string& pattern, size_t pos) {
    if (pos + pattern.length() > source_.length()) {
        return false;
    }
    
    for (size_t i = 0; i < pattern.length(); ++i) {
        if (source_[pos + i] != pattern[i]) {
            return false;
        }
    }
    
    // 确保模式后面不是字母数字字符（避免部分匹配）
    if (pos + pattern.length() < source_.length()) {
        char next_char = source_[pos + pattern.length()];
        if (is_alnum(next_char) || next_char == '_') {
            return false;
        }
    }
    
    return true;
}

bool UnifiedScanner::match_pattern_ignore_case(const std::string& pattern, size_t pos) {
    if (pos + pattern.length() > source_.length()) {
        return false;
    }
    
    for (size_t i = 0; i < pattern.length(); ++i) {
        if (std::tolower(source_[pos + i]) != std::tolower(pattern[i])) {
            return false;
        }
    }
    
    return true;
}

void UnifiedScanner::push_context(FragmentType type) {
    context_stack_.push_back(type);
}

void UnifiedScanner::pop_context() {
    if (!context_stack_.empty()) {
        context_stack_.pop_back();
    }
}

FragmentType UnifiedScanner::current_context() const {
    return context_stack_.empty() ? FragmentType::CHTL : context_stack_.back();
}

bool UnifiedScanner::is_in_context(FragmentType type) const {
    return std::find(context_stack_.begin(), context_stack_.end(), type) != context_stack_.end();
}

} // namespace scanner
} // namespace chtl