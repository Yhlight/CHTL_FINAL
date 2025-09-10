#include "scanner/unified_scanner.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stack>
#include <queue>
#include <regex>

namespace chtl {
namespace scanner {

UnifiedScanner::UnifiedScanner(const std::string& source) 
    : source_(source), position_(0), line_(1), column_(1), placeholder_counter_(0),
      current_state_(ScanState::NORMAL), in_global_style_(false), in_local_style_(false), 
      in_script_block_(false), in_chtl_js_block_(false) {
    initialize_boundaries();
    judgment_context_.is_wide_judgment = true;
    judgment_context_.is_strict_judgment = false;
    judgment_context_.expected_type = FragmentType::UNKNOWN;
}

void UnifiedScanner::initialize_boundaries() {
    initialize_chtl_boundaries();
    initialize_js_boundaries();
    initialize_css_boundaries();
    initialize_chtl_js_boundaries();
    
    // 按优先级排序边界
    std::sort(boundaries_.begin(), boundaries_.end(), 
              [](const SyntaxBoundary& a, const SyntaxBoundary& b) {
                  return a.priority > b.priority;
              });
    
    // 构建边界映射
    for (const auto& boundary : boundaries_) {
        boundary_map_[boundary.boundary_type].push_back(boundary);
    }
}

void UnifiedScanner::initialize_chtl_boundaries() {
    // CHTL语法边界 - 高优先级
    boundaries_.emplace_back(R"(\bstyle\s*\{)", R"(\})", FragmentType::CSS, BoundaryType::STYLE_BLOCK, true, 100);
    boundaries_.emplace_back(R"(\bscript\s*\{)", R"(\})", FragmentType::CHTL_JS, BoundaryType::SCRIPT_BLOCK, true, 100);
    
    // CHTL块边界
    boundaries_.emplace_back(R"(\b(html|head|body|div|span|p|h[1-6]|a|img|ul|ol|li|table|tr|td|th|form|input|button)\s*\{)", 
                            R"(\})", FragmentType::CHTL, BoundaryType::CHTL_BLOCK, true, 90);
    
    // 模板和自定义边界
    boundaries_.emplace_back(R"(\[Template\]\s+@Style\s+\w+)", R"(\})", FragmentType::CHTL, BoundaryType::CHTL_BLOCK, true, 95);
    boundaries_.emplace_back(R"(\[Template\]\s+@Element\s+\w+)", R"(\})", FragmentType::CHTL, BoundaryType::CHTL_BLOCK, true, 95);
    boundaries_.emplace_back(R"(\[Template\]\s+@Var\s+\w+)", R"(\})", FragmentType::CHTL, BoundaryType::CHTL_BLOCK, true, 95);
    boundaries_.emplace_back(R"(\[Custom\]\s+@Style\s+\w+)", R"(\})", FragmentType::CHTL, BoundaryType::CHTL_BLOCK, true, 95);
    boundaries_.emplace_back(R"(\[Custom\]\s+@Element\s+\w+)", R"(\})", FragmentType::CHTL, BoundaryType::CHTL_BLOCK, true, 95);
    boundaries_.emplace_back(R"(\[Custom\]\s+@Var\s+\w+)", R"(\})", FragmentType::CHTL, BoundaryType::CHTL_BLOCK, true, 95);
    
    // 原始嵌入边界
    boundaries_.emplace_back(R"(\[Origin\]\s+@Html)", R"(\})", FragmentType::HTML, BoundaryType::CHTL_BLOCK, true, 80);
    boundaries_.emplace_back(R"(\[Origin\]\s+@Style)", R"(\})", FragmentType::CSS, BoundaryType::CHTL_BLOCK, true, 80);
    boundaries_.emplace_back(R"(\[Origin\]\s+@JavaScript)", R"(\})", FragmentType::JS, BoundaryType::CHTL_BLOCK, true, 80);
    
    // 导入边界
    boundaries_.emplace_back(R"(\[Import\]\s+\w+\s+from\s+[\"'][^\"']+[\"'])", R"(\})", FragmentType::CHTL, BoundaryType::CHTL_BLOCK, true, 85);
}

void UnifiedScanner::initialize_js_boundaries() {
    // JavaScript语法边界
    boundaries_.emplace_back(R"(\bfunction\s+\w+\s*\()", R"(\})", FragmentType::JS, BoundaryType::JS_BLOCK, true, 70);
    boundaries_.emplace_back(R"(\bif\s*\()", R"(\})", FragmentType::JS, BoundaryType::JS_BLOCK, true, 60);
    boundaries_.emplace_back(R"(\bfor\s*\()", R"(\})", FragmentType::JS, BoundaryType::JS_BLOCK, true, 60);
    boundaries_.emplace_back(R"(\bwhile\s*\()", R"(\})", FragmentType::JS, BoundaryType::JS_BLOCK, true, 60);
    boundaries_.emplace_back(R"(\bswitch\s*\()", R"(\})", FragmentType::JS, BoundaryType::JS_BLOCK, true, 60);
    boundaries_.emplace_back(R"(\btry\s*\{)", R"(\})", FragmentType::JS, BoundaryType::JS_BLOCK, true, 60);
    boundaries_.emplace_back(R"(\bcatch\s*\()", R"(\})", FragmentType::JS, BoundaryType::JS_BLOCK, true, 60);
    boundaries_.emplace_back(R"(\bfinally\s*\{)", R"(\})", FragmentType::JS, BoundaryType::JS_BLOCK, true, 60);
    boundaries_.emplace_back(R"(\bclass\s+\w+)", R"(\})", FragmentType::JS, BoundaryType::JS_BLOCK, true, 60);
    
    // 对象和数组边界
    boundaries_.emplace_back(R"(\{)", R"(\})", FragmentType::JS, BoundaryType::JS_BLOCK, true, 50);
    boundaries_.emplace_back(R"(\[)", R"(\])", FragmentType::JS, BoundaryType::JS_BLOCK, true, 50);
}

void UnifiedScanner::initialize_css_boundaries() {
    // CSS语法边界
    boundaries_.emplace_back(R"(\.[\w-]+\s*\{)", R"(\})", FragmentType::CSS, BoundaryType::STYLE_BLOCK, true, 80);
    boundaries_.emplace_back(R"(#[\w-]+\s*\{)", R"(\})", FragmentType::CSS, BoundaryType::STYLE_BLOCK, true, 80);
    boundaries_.emplace_back(R"(\w+\s*\{)", R"(\})", FragmentType::CSS, BoundaryType::STYLE_BLOCK, true, 70);
    boundaries_.emplace_back(R"(@media\s+[^{]+\{)", R"(\})", FragmentType::CSS, BoundaryType::STYLE_BLOCK, true, 90);
    boundaries_.emplace_back(R"(@keyframes\s+\w+\s*\{)", R"(\})", FragmentType::CSS, BoundaryType::STYLE_BLOCK, true, 90);
    boundaries_.emplace_back(R"(@import\s+[^;]+;)", R"(\})", FragmentType::CSS, BoundaryType::STYLE_BLOCK, false, 85);
    boundaries_.emplace_back(R"(@charset\s+[^;]+;)", R"(\})", FragmentType::CSS, BoundaryType::STYLE_BLOCK, false, 85);
}

void UnifiedScanner::initialize_chtl_js_boundaries() {
    // CHTL JS语法边界 - 最高优先级
    boundaries_.emplace_back(R"(\bfileloader\s*\{)", R"(\})", FragmentType::CHTL_JS, BoundaryType::CHTL_JS_BLOCK, true, 200);
    boundaries_.emplace_back(R"(\blisten\s*\{)", R"(\})", FragmentType::CHTL_JS, BoundaryType::CHTL_JS_BLOCK, true, 200);
    boundaries_.emplace_back(R"(\bdelegate\s*\{)", R"(\})", FragmentType::CHTL_JS, BoundaryType::CHTL_JS_BLOCK, true, 200);
    boundaries_.emplace_back(R"(\banimate\s*\{)", R"(\})", FragmentType::CHTL_JS, BoundaryType::CHTL_JS_BLOCK, true, 200);
    boundaries_.emplace_back(R"(\bvir\s+\w+\s*=\s*)", R"(\})", FragmentType::CHTL_JS, BoundaryType::CHTL_JS_BLOCK, true, 200);
    boundaries_.emplace_back(R"(\brouter\s*\{)", R"(\})", FragmentType::CHTL_JS, BoundaryType::CHTL_JS_BLOCK, true, 200);
    boundaries_.emplace_back(R"(\biNeverAway\s*\{)", R"(\})", FragmentType::CHTL_JS, BoundaryType::CHTL_JS_BLOCK, true, 200);
    
    // CHTL JS选择器语法
    boundaries_.emplace_back(R"(\{\{[\w\s\.#\[\]()]+\}\})", R"(\})", FragmentType::CHTL_JS, BoundaryType::CHTL_JS_BLOCK, false, 150);
}

std::vector<CodeFragment> UnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    position_ = 0;
    line_ = 1;
    column_ = 1;

    while (position_ < source_.length()) {
        // Find the next boundary that is a block (`style {` or `script {`)
        size_t next_style_pos = source_.find("style", position_);
        size_t next_script_pos = source_.find("script", position_);

        size_t next_boundary_pos = std::string::npos;
        bool is_style = false;

        if (next_style_pos != std::string::npos && (next_script_pos == std::string::npos || next_style_pos < next_script_pos)) {
            next_boundary_pos = next_style_pos;
            is_style = true;
        } else if (next_script_pos != std::string::npos) {
            next_boundary_pos = next_script_pos;
            is_style = false;
        }

        // If a boundary is found
        if (next_boundary_pos != std::string::npos) {
            // Add the CHTL fragment before the boundary
            if (next_boundary_pos > position_) {
                fragments.emplace_back(FragmentType::CHTL, source_.substr(position_, next_boundary_pos - position_), position_, next_boundary_pos, line_, column_);
            }

            // Find the opening brace of the block
            size_t brace_pos = source_.find('{', next_boundary_pos);
            if (brace_pos == std::string::npos) {
                // Malformed block, treat the rest as CHTL
                position_ = source_.length();
                break;
            }

            // Find the matching closing brace
            size_t end_brace_pos = find_matching_end_brace(source_, brace_pos);
            if (end_brace_pos == std::string::npos) {
                // Malformed block, treat the rest as CHTL
                position_ = source_.length();
                break;
            }

            // Extract the block content
            size_t block_start_pos = next_boundary_pos;
            size_t block_end_pos = end_brace_pos + 1;
            std::string block_content = source_.substr(block_start_pos, block_end_pos - block_start_pos);

            // Determine the fragment type and add it
            FragmentType type = is_style ? FragmentType::CSS : (wide_judgment(block_content, FragmentType::CHTL_JS) ? FragmentType::CHTL_JS : FragmentType::JS);
            fragments.emplace_back(type, block_content, block_start_pos, block_end_pos, line_, column_);

            // Update position and line/column counts
            for(size_t i = position_; i < block_end_pos; ++i) {
                if(source_[i] == '\n') {
                    line_++;
                    column_ = 1;
                } else {
                    column_++;
                }
            }
            position_ = block_end_pos;

        } else {
            // No more boundaries, the rest is CHTL
            break;
        }
    }

    // Add any remaining CHTL content at the end of the file
    if (position_ < source_.length()) {
        fragments.emplace_back(FragmentType::CHTL, source_.substr(position_), position_, source_.length(), line_, column_);
    }

    return fragments;
}

// This entire block of old helper methods is being replaced by the logic within the new scan() method.
// The new approach is simpler and doesn't require these separate processing steps.
// The judgment logic is now a single call within scan(), and placeholder logic is not needed at this stage.

size_t UnifiedScanner::find_matching_end_brace(const std::string& content, size_t start_pos) {
    std::stack<char>
        s;
    bool in_string = false;
    char string_char = 0;
    bool in_comment = false;

    for (size_t i = start_pos; i < content.length(); ++i) {
        char c = content[i];
        char next_c = (i + 1 < content.length()) ? content[i + 1] : '\0';

        if (in_string) {
            if (c == '\\') {
                i++; // Skip escaped character
            } else if (c == string_char) {
                in_string = false;
            }
            continue;
        }
        
        if (in_comment) {
            if (c == '*' && next_c == '/') {
                in_comment = false;
                i++; // Skip '/'
            }
            continue;
        }
        
        if (c == '/' && next_c == '/') { // Single line comment
             while (i < content.length() && content[i] != '\n') {
                i++;
            }
            continue;
        }

        if (c == '/' && next_c == '*') { // Multi-line comment
            in_comment = true;
            i++; // Skip '*'
            continue;
        }

        if (c == '"' || c == '\'') {
            in_string = true;
            string_char = c;
            continue;
        }

        if (c == '{') {
            s.push('{');
        } else if (c == '}') {
            if (s.empty()) {
                return std::string::npos; // Unmatched closing brace
            }
            if (s.top() == '{') {
                s.pop();
                if (s.empty()) {
                    return i; // Found the matching brace
                }
            }
        }
    }
    
    return std::string::npos; // No matching brace found
}

bool UnifiedScanner::wide_judgment(const std::string& content, FragmentType type) {
    if (type != FragmentType::CHTL_JS) {
        return false;
    }
    // More robustly check for CHTL JS keywords, ensuring they are not part of other words.
    const std::vector<std::string> chtl_js_keywords = {
        "fileloader", "listen", "delegate", "animate", "vir", "router", "iNeverAway", "util"
    };

    for(const auto& keyword : chtl_js_keywords) {
        std::regex r("\\b" + keyword + "\\b");
        if (std::regex_search(content, r)) {
            return true;
        }
    }

    // Check for the selector syntax {{...}}
    if (content.find("{{") != std::string::npos) {
        return true;
    }

    return false;
}

// strict_judgment can be considered for future enhancement, but is not essential for the new scan logic.
bool UnifiedScanner::strict_judgment(const std::string& content, FragmentType type) {
    // For now, wide_judgment is sufficient.
    return wide_judgment(content, type);
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

std::string UnifiedScanner::get_scan_debug_info() const {
    std::ostringstream oss;
    oss << "=== 统一扫描器调试信息 ===" << std::endl;
    oss << "当前状态: " << static_cast<int>(current_state_) << std::endl;
    oss << "位置: " << position_ << " (行 " << line_ << ", 列 " << column_ << ")" << std::endl;
    oss << "占位符数量: " << placeholders_.size() << std::endl;
    oss << "在样式块中: " << (in_style_block_ ? "是" : "否") << std::endl;
    oss << "在脚本块中: " << (in_script_block_ ? "是" : "否") << std::endl;
    oss << "在CHTL JS块中: " << (in_chtl_js_block_ ? "是" : "否") << std::endl;
    return oss.str();
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

// 可变长度切片与智能扩增
std::pair<size_t, size_t> UnifiedScanner::find_optimal_boundary(const SyntaxBoundary& boundary, size_t start_pos) {
    auto range = find_boundary_range(boundary, start_pos);
    if (should_expand_boundary(range.first, range.second, boundary.type)) {
        expand_boundary(range.first, range.second, boundary.type);
        record_expansion(range.first, range.second, boundary.type, "Unterminated ternary operator");
    }
    return range;
}

bool UnifiedScanner::should_expand_boundary(size_t start_pos, size_t end_pos, FragmentType type) {
    if (type != FragmentType::CSS && type != FragmentType::CHTL) {
        return false;
    }
    std::string fragment = source_.substr(start_pos, end_pos - start_pos);
    size_t question_mark = fragment.rfind('?');
    if (question_mark != std::string::npos) {
        size_t colon = fragment.rfind(':');
        if (colon == std::string::npos || colon < question_mark) {
            // Found a '?' without a following ':'
            return true;
        }
    }
    return false;
}

void UnifiedScanner::expand_boundary(size_t& start_pos, size_t& end_pos, FragmentType type) {
    // Expand until we find a semicolon or a closing brace
    size_t original_end = end_pos;
    while (end_pos < source_.length()) {
        if (source_[end_pos] == ';' || source_[end_pos] == '}') {
            end_pos++; // include the terminator
            break;
        }
        end_pos++;
    }
    if (end_pos == original_end) {
        // Did not find a terminator, reset to original
        end_pos = original_end;
    }
}

void UnifiedScanner::record_expansion(size_t start_pos, size_t end_pos, FragmentType type, const std::string& reason) {
    expansion_history_.push_back({start_pos, end_pos, type, 100, reason});
}

std::pair<size_t, size_t> UnifiedScanner::find_boundary_range(const SyntaxBoundary& boundary, size_t start_pos) {
    // A more sophisticated implementation would use the boundary's end_pattern regex.
    // For now, we'll use the brace matching logic as a default.
    size_t end_pos = find_matching_end_brace(source_, start_pos);
    if (end_pos == std::string::npos) {
        return {start_pos, std::string::npos};
    }
    return {start_pos, end_pos};
}


} // namespace scanner
} // namespace chtl