#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <stack>
#include <regex>

namespace chtl {
namespace scanner {

// 代码片段类型
enum class FragmentType {
    CHTL,           // CHTL代码
    CHTL_JS,        // CHTL JS代码
    JS,             // 纯JavaScript代码
    CSS,            // CSS代码
    HTML,           // HTML代码
    UNKNOWN         // 未知类型
};

// 扫描状态
enum class ScanState {
    NORMAL,         // 正常状态
    IN_CHTL_BLOCK,  // 在CHTL块中
    IN_STYLE_BLOCK, // 在样式块中
    IN_SCRIPT_BLOCK,// 在脚本块中
    IN_CHTL_JS,     // 在CHTL JS中
    IN_JS,          // 在JS中
    IN_PLACEHOLDER  // 在占位符中
};

// 语法边界类型
enum class BoundaryType {
    CHTL_BLOCK,     // CHTL块边界
    STYLE_BLOCK,    // 样式块边界
    SCRIPT_BLOCK,   // 脚本块边界
    CHTL_JS_BLOCK,  // CHTL JS块边界
    JS_BLOCK,       // JS块边界
    PLACEHOLDER     // 占位符边界
};

// 代码片段
struct CodeFragment {
    FragmentType type;
    std::string content;
    size_t start_pos;
    size_t end_pos;
    int line;
    int column;
    
    CodeFragment(FragmentType t, const std::string& c, size_t start, size_t end, int l = 1, int col = 1)
        : type(t), content(c), start_pos(start), end_pos(end), line(l), column(col) {}
};

// 占位符信息（增强版）
struct PlaceholderInfo {
    std::string placeholder;
    FragmentType original_type;
    std::string original_content;
    size_t start_pos;
    size_t end_pos;
    int line;
    int column;
    ScanState context_state;  // 占位符创建时的上下文状态
    std::string parent_placeholder;  // 父占位符（用于嵌套）
    bool is_restored;  // 是否已恢复
    
    // 默认构造函数
    PlaceholderInfo() : original_type(FragmentType::UNKNOWN), start_pos(0), end_pos(0), 
                       line(1), column(1), context_state(ScanState::NORMAL), is_restored(false) {}
    
    PlaceholderInfo(const std::string& ph, FragmentType type, const std::string& content, 
                   size_t start, size_t end, int l = 1, int col = 1, ScanState state = ScanState::NORMAL)
        : placeholder(ph), original_type(type), original_content(content), 
          start_pos(start), end_pos(end), line(l), column(col), context_state(state), is_restored(false) {}
          
    // 添加拷贝构造函数和赋值操作符
    PlaceholderInfo(const PlaceholderInfo& other) = default;
    PlaceholderInfo(PlaceholderInfo&& other) = default;
    PlaceholderInfo& operator=(const PlaceholderInfo& other) = default;
    PlaceholderInfo& operator=(PlaceholderInfo&& other) = default;
    
    // 添加swap函数
    void swap(PlaceholderInfo& other) noexcept {
        std::swap(placeholder, other.placeholder);
        std::swap(original_type, other.original_type);
        std::swap(original_content, other.original_content);
        std::swap(start_pos, other.start_pos);
        std::swap(end_pos, other.end_pos);
        std::swap(line, other.line);
        std::swap(column, other.column);
        std::swap(context_state, other.context_state);
        std::swap(parent_placeholder, other.parent_placeholder);
        std::swap(is_restored, other.is_restored);
    }
    
    // 添加全局swap函数
    friend void swap(PlaceholderInfo& a, PlaceholderInfo& b) noexcept {
        a.swap(b);
    }
    
    // 添加比较操作符
    bool operator<(const PlaceholderInfo& other) const {
        return start_pos < other.start_pos;
    }
    
    bool operator==(const PlaceholderInfo& other) const {
        return placeholder == other.placeholder && 
               original_type == other.original_type &&
               original_content == other.original_content &&
               start_pos == other.start_pos &&
               end_pos == other.end_pos &&
               line == other.line &&
               column == other.column;
    }
    
    // 添加其他比较操作符
    bool operator!=(const PlaceholderInfo& other) const {
        return !(*this == other);
    }
    
    bool operator<=(const PlaceholderInfo& other) const {
        return *this < other || *this == other;
    }
    
    bool operator>(const PlaceholderInfo& other) const {
        return !(*this <= other);
    }
    
    bool operator>=(const PlaceholderInfo& other) const {
        return !(*this < other);
    }
    
    // 添加hash支持
    struct Hash {
        std::size_t operator()(const PlaceholderInfo& info) const {
            return std::hash<std::string>{}(info.placeholder) ^
                   (std::hash<int>{}(static_cast<int>(info.original_type)) << 1) ^
                   (std::hash<std::string>{}(info.original_content) << 2) ^
                   (std::hash<size_t>{}(info.start_pos) << 3) ^
                   (std::hash<size_t>{}(info.end_pos) << 4) ^
                   (std::hash<int>{}(info.line) << 5) ^
                   (std::hash<int>{}(info.column) << 6);
        }
    };
};

// 语法边界定义
struct SyntaxBoundary {
    std::string start_pattern;
    std::string end_pattern;
    FragmentType type;
    BoundaryType boundary_type;
    bool is_nested;  // 是否支持嵌套
    int priority;    // 优先级，数字越大优先级越高
    std::regex start_regex;
    std::regex end_regex;
    
    SyntaxBoundary(const std::string& start, const std::string& end, FragmentType t, 
                   BoundaryType bt, bool nested = false, int prio = 0)
        : start_pattern(start), end_pattern(end), type(t), boundary_type(bt), 
          is_nested(nested), priority(prio) {
        start_regex = std::regex(start_pattern);
        end_regex = std::regex(end_pattern);
    }
};

// 统一扫描器类

class UnifiedScanner {
private:
    std::string source_;
    size_t position_;
    int line_;
    int column_;
    
    // 扫描状态管理
    ScanState current_state_;
    std::stack<ScanState> state_stack_;
    std::stack<size_t> position_stack_;
    
    // 占位符管理
    std::vector<PlaceholderInfo> placeholders_;
    std::unordered_map<std::string, PlaceholderInfo> placeholder_map_;
    std::unordered_map<std::string, std::vector<std::string>> nested_placeholders_;
    size_t placeholder_counter_;
    
    // 语法边界定义
    std::vector<SyntaxBoundary> boundaries_;
    std::unordered_map<BoundaryType, std::vector<SyntaxBoundary>> boundary_map_;
    
    // 扫描上下文
    std::vector<FragmentType> context_stack_;
    std::string current_block_type_;
    bool in_global_style_;
    bool in_local_style_;
    bool in_script_block_;
    bool in_style_block_;
    bool in_chtl_js_block_;
    
    // 智能扩增相关
    struct ExpansionInfo {
        size_t start_pos;
        size_t end_pos;
        FragmentType type;
        int confidence;  // 置信度
        std::string reason;  // 扩增原因
    };
    std::vector<ExpansionInfo> expansion_history_;
    
    // 宽判严判相关
    struct JudgmentContext {
        bool is_wide_judgment;
        bool is_strict_judgment;
        FragmentType expected_type;
        std::string context_hint;
    };
    JudgmentContext judgment_context_;
    
    // 初始化方法
    void initialize_boundaries();
    void initialize_chtl_boundaries();
    void initialize_js_boundaries();
    void initialize_css_boundaries();
    void initialize_chtl_js_boundaries();
    
    // 核心扫描方法
    std::vector<CodeFragment> scan_fragments();
    CodeFragment scan_chtl_fragment();
    CodeFragment scan_style_fragment();
    CodeFragment scan_script_fragment();
    CodeFragment scan_chtl_js_fragment();
    CodeFragment scan_js_fragment();
    
    // 语法边界识别（增强版）
    bool is_syntax_boundary(const std::string& pattern, size_t pos);
    bool is_style_boundary(size_t pos);
    bool is_script_boundary(size_t pos);
    bool is_chtl_js_boundary(size_t pos);
    bool is_js_boundary(size_t pos);
    bool is_placeholder_boundary(size_t pos);
    
    // 占位符机制（完整实现）
    std::string create_placeholder(const std::string& prefix = "_JS_CODE_PLACEHOLDER_");
    void replace_with_placeholder(const CodeFragment& fragment, const std::string& parent_placeholder = "");
    std::string restore_placeholders(const std::string& processed_content);
    void restore_nested_placeholders();
    bool is_placeholder(const std::string& text) const;
    
    // 可变长度切片与智能扩增
    std::pair<size_t, size_t> find_optimal_boundary(const SyntaxBoundary& boundary, size_t start_pos);
    bool should_expand_boundary(size_t start_pos, size_t end_pos, FragmentType type);
    void expand_boundary(size_t& start_pos, size_t& end_pos, FragmentType type);
    void record_expansion(size_t start_pos, size_t end_pos, FragmentType type, const std::string& reason);
    
    // 双指针扫描（增强版）
    std::pair<size_t, size_t> find_boundary_range(const SyntaxBoundary& boundary, size_t start_pos);
    bool is_valid_boundary(const SyntaxBoundary& boundary, size_t start_pos, size_t end_pos);
    std::vector<SyntaxBoundary> find_matching_boundaries(size_t pos);
    
    // 宽判严判机制（完整实现）
    bool wide_judgment(const std::string& content, FragmentType type);
    bool strict_judgment(const std::string& content, FragmentType type);
    void set_judgment_context(bool wide, bool strict, FragmentType expected, const std::string& hint);
    bool is_chtl_syntax_allowed_in_context(const std::string& content, ScanState state);
    
    // 状态管理
    void push_state(ScanState state);
    void pop_state();
    void set_state(ScanState state);
    ScanState get_state() const { return current_state_; }
    
    // 上下文管理
    void push_context(FragmentType type);
    void pop_context();
    FragmentType current_context() const;
    bool is_in_context(FragmentType type) const;
    
    // CHTL JS和JS分离（核心功能）
    std::string separate_chtl_js_and_js(const std::string& content);
    std::string process_js_placeholders(const std::string& content);
    std::string process_chtl_js_syntax(const std::string& content);
    
    // 工具方法
    char current_char() const;
    char peek_char(size_t offset = 1) const;
    void advance();
    void skip_whitespace();
    bool is_whitespace(char c) const;
    bool is_alpha(char c) const;
    bool is_digit(char c) const;
    bool is_alnum(char c) const;
    size_t find_matching_end_brace(const std::string& content, size_t start_pos);
    
    // 模式匹配（增强版）
    bool match_pattern(const std::string& pattern, size_t pos);
    bool match_pattern_ignore_case(const std::string& pattern, size_t pos);
    bool match_regex(const std::regex& pattern, size_t pos);
    std::smatch match_regex_with_result(const std::regex& pattern, size_t pos);
    
    // 嵌套处理（增强版）
    bool handle_nested_syntax(const SyntaxBoundary& boundary, size_t start_pos);
    std::pair<size_t, size_t> find_nested_boundary(const SyntaxBoundary& boundary, size_t start_pos);
    bool is_nested_placeholder(const std::string& placeholder) const;
    
    
public:
    explicit UnifiedScanner(const std::string& source);
    
    // 主要接口
    std::vector<CodeFragment> scan();
    std::string process_for_chtl_compiler();
    std::string process_for_chtl_js_compiler();
    std::string process_for_js_compiler();
    std::string process_for_css_compiler();
    
    // 分离处理接口
    std::string separate_global_style_blocks();
    std::string separate_local_script_chtl_syntax();
    std::string separate_chtl_js_and_js_completely();
    
    // 占位符管理
    const std::vector<PlaceholderInfo>& get_placeholders() const { return placeholders_; }
    const std::unordered_map<std::string, PlaceholderInfo>& get_placeholder_map() const { return placeholder_map_; }
    const std::unordered_map<std::string, std::vector<std::string>>& get_nested_placeholders() const { return nested_placeholders_; }
    
    // 状态查询
    ScanState get_current_state() const { return current_state_; }
    bool is_in_style_block() const { return in_local_style_ || in_global_style_; }
    bool is_in_script_block() const { return in_script_block_; }
    bool is_in_chtl_js_block() const { return in_chtl_js_block_; }
    
    // 调试和诊断
    void print_fragments() const;
    void print_placeholders() const;
    void print_scan_state() const;
    void print_expansion_history() const;
    std::string get_processed_content();
    std::string get_scan_debug_info() const;
    
    // 重置状态
    void reset();
    void clear_placeholders();
    void clear_expansion_history();
    
    // 配置选项
    void set_wide_judgment_enabled(bool enabled);
    void set_strict_judgment_enabled(bool enabled);
    void set_placeholder_prefix(const std::string& prefix);
};

} // namespace scanner
} // namespace chtl