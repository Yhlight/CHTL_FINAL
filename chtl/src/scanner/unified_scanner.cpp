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
    context_stack_.clear();
    in_style_block_ = false;
    in_script_block_ = false;
    in_chtl_js_block_ = false;
    
    // 首先进行CHTL JS和JS分离
    std::string processed_source = separate_chtl_js_and_js_completely();
    
    // 然后对处理后的源代码进行常规扫描
    std::string original_source = source_;
    source_ = processed_source;
    
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
    
    // 恢复原始源代码
    source_ = original_source;
    
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

std::string UnifiedScanner::create_placeholder(const std::string& prefix) {
    std::string placeholder = prefix + std::to_string(placeholder_counter_++) + "_";
    return placeholder;
}

void UnifiedScanner::replace_with_placeholder(const CodeFragment& fragment, const std::string& parent_placeholder) {
    std::string placeholder = create_placeholder("_JS_CODE_PLACEHOLDER_");
    PlaceholderInfo info(placeholder, fragment.type, fragment.content, 
                        fragment.start_pos, fragment.end_pos, fragment.line, fragment.column, current_state_);
    
    if (!parent_placeholder.empty()) {
        info.parent_placeholder = parent_placeholder;
        nested_placeholders_[parent_placeholder].push_back(placeholder);
    }
    
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
        if (placeholder.is_restored) continue;
        
        size_t pos = result.find(placeholder.placeholder);
        if (pos != std::string::npos) {
            result.replace(pos, placeholder.placeholder.length(), placeholder.original_content);
            // 标记为已恢复
            const_cast<PlaceholderInfo&>(placeholder).is_restored = true;
        }
    }
    
    return result;
}

// 核心功能：完全分离CHTL JS和JS代码
std::string UnifiedScanner::separate_chtl_js_and_js_completely() {
    std::string result = source_;
    
    // 第一步：识别并替换所有CHTL JS语法为占位符
    result = process_chtl_js_syntax(result);
    
    // 第二步：将剩余的JS代码替换为占位符
    result = process_js_placeholders(result);
    
    return result;
}

std::string UnifiedScanner::process_chtl_js_syntax(const std::string& content) {
    std::string result = content;
    
    // 处理CHTL JS语法：fileloader, listen, animate, vir, router等
    std::vector<std::pair<size_t, size_t>> chtl_js_blocks;
    
    // 查找所有CHTL JS块
    for (const auto& boundary : boundary_map_[BoundaryType::CHTL_JS_BLOCK]) {
        std::regex pattern(boundary.start_pattern);
        std::sregex_iterator begin(content.begin(), content.end(), pattern);
        std::sregex_iterator end;
        
        for (auto it = begin; it != end; ++it) {
            size_t start_pos = it->position();
            auto range = find_optimal_boundary(boundary, start_pos);
            size_t end_pos = range.second;
            if (end_pos != std::string::npos) {
                chtl_js_blocks.push_back({start_pos, end_pos});
            }
        }
    }
    
    // 按位置倒序处理，避免位置偏移
    std::sort(chtl_js_blocks.begin(), chtl_js_blocks.end(), 
              [](const std::pair<size_t, size_t>& a, const std::pair<size_t, size_t>& b) {
                  return a.first > b.first;
              });
    
    // 替换CHTL JS块为占位符
    for (const auto& block : chtl_js_blocks) {
        std::string chtl_js_content = content.substr(block.first, block.second - block.first + 1);
        std::string placeholder = create_placeholder("_CHTL_JS_PLACEHOLDER_");
        
        PlaceholderInfo info(placeholder, FragmentType::CHTL_JS, chtl_js_content,
                           block.first, block.second, 1, 1, current_state_);
        placeholders_.push_back(info);
        placeholder_map_[placeholder] = info;
        
        result.replace(block.first, block.second - block.first + 1, placeholder);
    }
    
    return result;
}

std::string UnifiedScanner::process_js_placeholders(const std::string& content) {
    std::string result = content;
    
    // 查找所有JS块（函数、对象、数组等）
    std::vector<std::pair<size_t, size_t>> js_blocks;
    
    for (const auto& boundary : boundary_map_[BoundaryType::JS_BLOCK]) {
        std::regex pattern(boundary.start_pattern);
        std::sregex_iterator begin(content.begin(), content.end(), pattern);
        std::sregex_iterator end;
        
        for (auto it = begin; it != end; ++it) {
            size_t start_pos = it->position();
            size_t end_pos = find_matching_end_brace(content, start_pos);
            if (end_pos != std::string::npos) {
                js_blocks.push_back({start_pos, end_pos});
            }
        }
    }
    
    // 按位置倒序处理
    std::sort(js_blocks.begin(), js_blocks.end(), 
              [](const std::pair<size_t, size_t>& a, const std::pair<size_t, size_t>& b) {
                  return a.first > b.first;
              });
    
    // 替换JS块为占位符
    for (const auto& block : js_blocks) {
        std::string js_content = content.substr(block.first, block.second - block.first + 1);
        std::string placeholder = create_placeholder("_JS_CODE_PLACEHOLDER_");
        
        PlaceholderInfo info(placeholder, FragmentType::JS, js_content,
                           block.first, block.second, 1, 1, current_state_);
        placeholders_.push_back(info);
        placeholder_map_[placeholder] = info;
        
        result.replace(block.first, block.second - block.first + 1, placeholder);
    }
    
    return result;
}

size_t UnifiedScanner::find_matching_end_brace(const std::string& content, size_t start_pos) {
    int brace_count = 0;
    bool in_string = false;
    char string_char = 0;
    
    for (size_t i = start_pos; i < content.length(); ++i) {
        char c = content[i];
        
        if (!in_string) {
            if (c == '"' || c == '\'') {
                in_string = true;
                string_char = c;
            } else if (c == '{') {
                brace_count++;
            } else if (c == '}') {
                brace_count--;
                if (brace_count == 0) {
                    return i;
                }
            }
        } else {
            if (c == string_char && content[i-1] != '\\') {
                in_string = false;
            }
        }
    }
    
    return std::string::npos;
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