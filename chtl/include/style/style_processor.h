#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace chtl {
namespace style {

// 样式属性类型
enum class AttributeType {
    LITERAL,        // 字面量值
    REFERENCE,      // 引用其他属性
    EXPRESSION,     // 表达式
    CONDITIONAL     // 条件表达式
};

// 样式属性
struct StyleAttribute {
    std::string name;
    std::string value;
    AttributeType type;
    std::string expression;  // 用于存储表达式或条件
    std::vector<std::string> dependencies;  // 依赖的属性
    
    StyleAttribute(const std::string& n, const std::string& v, AttributeType t = AttributeType::LITERAL)
        : name(n), value(v), type(t) {}
};

// 样式规则
struct StyleRule {
    std::string selector;
    std::vector<StyleAttribute> attributes;
    std::string parent_selector;  // 父选择器，用于上下文推断
    int specificity;  // 选择器特异性
    
    StyleRule(const std::string& sel) : selector(sel), specificity(0) {}
};

// 样式上下文
class StyleContext {
private:
    std::unordered_map<std::string, std::string> attributes_;
    std::string current_element_;
    std::vector<std::string> element_stack_;
    
public:
    void set_attribute(const std::string& name, const std::string& value);
    std::string get_attribute(const std::string& name) const;
    bool has_attribute(const std::string& name) const;
    
    void set_current_element(const std::string& element);
    std::string get_current_element() const;
    
    void push_element(const std::string& element);
    void pop_element();
    
    // 上下文推断
    std::string infer_context_selector() const;
    std::string resolve_selector(const std::string& selector) const;
};

// 表达式求值器
class ExpressionEvaluator {
public:
    // 算术运算
    static std::string add(const std::string& a, const std::string& b);
    static std::string subtract(const std::string& a, const std::string& b);
    static std::string multiply(const std::string& a, const std::string& b);
    static std::string divide(const std::string& a, const std::string& b);
    
    // 比较运算
    static bool equals(const std::string& a, const std::string& b);
    static bool not_equals(const std::string& a, const std::string& b);
    static bool greater_than(const std::string& a, const std::string& b);
    static bool less_than(const std::string& a, const std::string& b);
    static bool greater_equal(const std::string& a, const std::string& b);
    static bool less_equal(const std::string& a, const std::string& b);
    
    // 逻辑运算
    static bool logical_and(bool a, bool b);
    static bool logical_or(bool a, bool b);
    static bool logical_not(bool a);
    
    // 条件表达式求值
    static std::string evaluate_conditional(const std::string& condition, 
                                          const std::string& true_value, 
                                          const std::string& false_value,
                                          const StyleContext& context);
    
    // 属性引用解析
    static std::string resolve_attribute_reference(const std::string& reference, 
                                                  const StyleContext& context);
    
    // 表达式解析和求值
    static std::string evaluate_expression(const std::string& expression, 
                                          const StyleContext& context);
};

// 样式处理器
class StyleProcessor {
private:
    std::vector<StyleRule> rules_;
    StyleContext context_;
    
    // 选择器处理
    std::string process_selector(const std::string& selector);
    int calculate_specificity(const std::string& selector);
    
    // 属性处理
    StyleAttribute process_attribute(const std::string& name, const std::string& value);
    std::string process_attribute_value(const std::string& value);
    
    // 上下文推断
    void infer_context_for_rule(StyleRule& rule);
    
    // 自动化类/ID添加
    void auto_add_class_id(StyleRule& rule);
    
public:
    StyleProcessor();
    
    // 主要接口
    void add_rule(const StyleRule& rule);
    void process_style_block(const std::string& style_content, const std::string& parent_selector = "");
    
    // 上下文管理
    void set_context_attribute(const std::string& name, const std::string& value);
    void set_current_element(const std::string& element);
    void push_element(const std::string& element);
    void pop_element();
    
    // 样式生成
    std::string generate_css() const;
    std::string generate_css_for_element(const std::string& element) const;
    
    // 调试
    void print_rules() const;
    void print_context() const;
    const StyleContext& get_context() const { return context_; }
    
    // 重置
    void clear();
    void reset_context();
};

} // namespace style
} // namespace chtl