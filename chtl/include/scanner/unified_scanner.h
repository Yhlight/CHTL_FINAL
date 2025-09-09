#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

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

// 占位符信息
struct PlaceholderInfo {
    std::string placeholder;
    FragmentType original_type;
    std::string original_content;
    size_t start_pos;
    size_t end_pos;
    int line;
    int column;
    
    // 默认构造函数
    PlaceholderInfo() : original_type(FragmentType::UNKNOWN), start_pos(0), end_pos(0), line(1), column(1) {}
    
    PlaceholderInfo(const std::string& ph, FragmentType type, const std::string& content, 
                   size_t start, size_t end, int l = 1, int col = 1)
        : placeholder(ph), original_type(type), original_content(content), 
          start_pos(start), end_pos(end), line(l), column(col) {}
          
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
    bool is_nested;  // 是否支持嵌套
    
    SyntaxBoundary(const std::string& start, const std::string& end, FragmentType t, bool nested = false)
        : start_pattern(start), end_pattern(end), type(t), is_nested(nested) {}
};

class UnifiedScanner {
private:
    std::string source_;
    size_t position_;
    int line_;
    int column_;
    
    // 占位符管理
    std::vector<PlaceholderInfo> placeholders_;
    std::unordered_map<std::string, PlaceholderInfo> placeholder_map_;
    size_t placeholder_counter_;
    
    // 语法边界定义
    std::vector<SyntaxBoundary> boundaries_;
    
    // 扫描状态
    std::vector<FragmentType> context_stack_;
    bool in_style_block_;
    bool in_script_block_;
    bool in_chtl_js_block_;
    
    void initialize_boundaries();
    void initialize_chtl_boundaries();
    void initialize_js_boundaries();
    void initialize_css_boundaries();
    
    // 核心扫描方法
    std::vector<CodeFragment> scan_fragments();
    CodeFragment scan_chtl_fragment();
    CodeFragment scan_style_fragment();
    CodeFragment scan_script_fragment();
    CodeFragment scan_chtl_js_fragment();
    CodeFragment scan_js_fragment();
    
    // 语法边界识别
    bool is_syntax_boundary(const std::string& pattern, size_t pos);
    bool is_style_boundary(size_t pos);
    bool is_script_boundary(size_t pos);
    bool is_chtl_js_boundary(size_t pos);
    bool is_js_boundary(size_t pos);
    
    // 占位符机制
    std::string create_placeholder();
    void replace_with_placeholder(const CodeFragment& fragment);
    std::string restore_placeholders(const std::string& processed_content);
    
    // 双指针扫描
    std::pair<size_t, size_t> find_boundary_range(const SyntaxBoundary& boundary, size_t start_pos);
    bool is_valid_boundary(const SyntaxBoundary& boundary, size_t start_pos, size_t end_pos);
    
    // 宽判严判机制
    bool wide_judgment(const std::string& content, FragmentType type);
    bool strict_judgment(const std::string& content, FragmentType type);
    
    // 上下文管理
    void push_context(FragmentType type);
    void pop_context();
    FragmentType current_context() const;
    bool is_in_context(FragmentType type) const;
    
    // 工具方法
    char current_char() const;
    char peek_char(size_t offset = 1) const;
    void advance();
    void skip_whitespace();
    bool is_whitespace(char c) const;
    bool is_alpha(char c) const;
    bool is_digit(char c) const;
    bool is_alnum(char c) const;
    
    // 模式匹配
    bool match_pattern(const std::string& pattern, size_t pos);
    bool match_pattern_ignore_case(const std::string& pattern, size_t pos);
    
    // 嵌套处理
    bool handle_nested_syntax(const SyntaxBoundary& boundary, size_t start_pos);
    std::pair<size_t, size_t> find_nested_boundary(const SyntaxBoundary& boundary, size_t start_pos);
    
public:
    explicit UnifiedScanner(const std::string& source);
    
    // 主要接口
    std::vector<CodeFragment> scan();
    std::string process_for_chtl_compiler();
    std::string process_for_chtl_js_compiler();
    std::string process_for_js_compiler();
    std::string process_for_css_compiler();
    
    // 占位符管理
    const std::vector<PlaceholderInfo>& get_placeholders() const { return placeholders_; }
    const std::unordered_map<std::string, PlaceholderInfo>& get_placeholder_map() const { return placeholder_map_; }
    
    // 调试和诊断
    void print_fragments() const;
    void print_placeholders() const;
    std::string get_processed_content();
    
    // 重置状态
    void reset();
    void clear_placeholders();
};

} // namespace scanner
} // namespace chtl