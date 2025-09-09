#include "style/style_processor.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <iostream>

namespace chtl {
namespace style {

// StyleContext 实现
void StyleContext::set_attribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

std::string StyleContext::get_attribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return (it != attributes_.end()) ? it->second : "";
}

bool StyleContext::has_attribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

void StyleContext::set_current_element(const std::string& element) {
    current_element_ = element;
}

std::string StyleContext::get_current_element() const {
    return current_element_;
}

void StyleContext::push_element(const std::string& element) {
    element_stack_.push_back(element);
    current_element_ = element;
}

void StyleContext::pop_element() {
    if (!element_stack_.empty()) {
        element_stack_.pop_back();
        current_element_ = element_stack_.empty() ? "" : element_stack_.back();
    }
}

std::string StyleContext::infer_context_selector() const {
    if (element_stack_.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < element_stack_.size(); ++i) {
        if (i > 0) oss << " ";
        oss << element_stack_[i];
    }
    return oss.str();
}

std::string StyleContext::resolve_selector(const std::string& selector) const {
    if (selector.empty()) {
        return infer_context_selector();
    }
    
    // 处理 & 符号，替换为当前上下文
    if (selector.find('&') != std::string::npos) {
        std::string context = infer_context_selector();
        std::string result = selector;
        size_t pos = 0;
        while ((pos = result.find('&', pos)) != std::string::npos) {
            result.replace(pos, 1, context);
            pos += context.length();
        }
        return result;
    }
    
    return selector;
}

// ExpressionEvaluator 实现
std::string ExpressionEvaluator::add(const std::string& a, const std::string& b) {
    try {
        double val_a = std::stod(a);
        double val_b = std::stod(b);
        return std::to_string(val_a + val_b);
    } catch (...) {
        return a + b;  // 字符串拼接
    }
}

std::string ExpressionEvaluator::subtract(const std::string& a, const std::string& b) {
    try {
        double val_a = std::stod(a);
        double val_b = std::stod(b);
        return std::to_string(val_a - val_b);
    } catch (...) {
        return a;  // 无法计算时返回原值
    }
}

std::string ExpressionEvaluator::multiply(const std::string& a, const std::string& b) {
    try {
        double val_a = std::stod(a);
        double val_b = std::stod(b);
        return std::to_string(val_a * val_b);
    } catch (...) {
        return a;  // 无法计算时返回原值
    }
}

std::string ExpressionEvaluator::divide(const std::string& a, const std::string& b) {
    try {
        double val_a = std::stod(a);
        double val_b = std::stod(b);
        if (val_b != 0) {
            return std::to_string(val_a / val_b);
        }
    } catch (...) {
        // 忽略错误
    }
    return a;  // 无法计算时返回原值
}

bool ExpressionEvaluator::equals(const std::string& a, const std::string& b) {
    return a == b;
}

bool ExpressionEvaluator::not_equals(const std::string& a, const std::string& b) {
    return a != b;
}

bool ExpressionEvaluator::greater_than(const std::string& a, const std::string& b) {
    try {
        double val_a = std::stod(a);
        double val_b = std::stod(b);
        return val_a > val_b;
    } catch (...) {
        return a > b;  // 字符串比较
    }
}

bool ExpressionEvaluator::less_than(const std::string& a, const std::string& b) {
    try {
        double val_a = std::stod(a);
        double val_b = std::stod(b);
        return val_a < val_b;
    } catch (...) {
        return a < b;  // 字符串比较
    }
}

bool ExpressionEvaluator::greater_equal(const std::string& a, const std::string& b) {
    return greater_than(a, b) || equals(a, b);
}

bool ExpressionEvaluator::less_equal(const std::string& a, const std::string& b) {
    return less_than(a, b) || equals(a, b);
}

bool ExpressionEvaluator::logical_and(bool a, bool b) {
    return a && b;
}

bool ExpressionEvaluator::logical_or(bool a, bool b) {
    return a || b;
}

bool ExpressionEvaluator::logical_not(bool a) {
    return !a;
}

std::string ExpressionEvaluator::evaluate_conditional(const std::string& condition, 
                                                    const std::string& true_value, 
                                                    const std::string& false_value,
                                                    const StyleContext& context) {
    // 简单的条件表达式求值
    // 这里可以实现更复杂的条件逻辑
    if (condition == "true" || condition == "1") {
        return true_value;
    }
    return false_value;
}

std::string ExpressionEvaluator::resolve_attribute_reference(const std::string& reference, 
                                                           const StyleContext& context) {
    // 处理属性引用，如 @width, @height 等
    if (reference.length() > 1 && reference[0] == '@') {
        std::string attr_name = reference.substr(1);
        return context.get_attribute(attr_name);
    }
    return reference;
}

std::string ExpressionEvaluator::evaluate_expression(const std::string& expression, 
                                                    const StyleContext& context) {
    // 简单的表达式求值
    // 这里可以实现更复杂的表达式解析
    std::string result = expression;
    
    // 处理属性引用
    std::regex attr_ref_regex(R"(@(\w+))");
    std::smatch match;
    while (std::regex_search(result, match, attr_ref_regex)) {
        std::string attr_name = match[1].str();
        std::string attr_value = context.get_attribute(attr_name);
        result = match.prefix().str() + attr_value + match.suffix().str();
    }
    
    return result;
}

// StyleProcessor 实现
StyleProcessor::StyleProcessor() {
    // 初始化
}

void StyleProcessor::add_rule(const StyleRule& rule) {
    rules_.push_back(rule);
}

void StyleProcessor::process_style_block(const std::string& style_content, const std::string& parent_selector) {
    // 这里应该实现CSS解析逻辑
    // 暂时简化处理
    StyleRule rule(parent_selector);
    rule.parent_selector = parent_selector;
    rules_.push_back(rule);
}

void StyleProcessor::set_context_attribute(const std::string& name, const std::string& value) {
    context_.set_attribute(name, value);
}

void StyleProcessor::set_current_element(const std::string& element) {
    context_.set_current_element(element);
}

void StyleProcessor::push_element(const std::string& element) {
    context_.push_element(element);
}

void StyleProcessor::pop_element() {
    context_.pop_element();
}

std::string StyleProcessor::generate_css() const {
    std::ostringstream oss;
    
    for (const auto& rule : rules_) {
        oss << rule.selector << " {\n";
        for (const auto& attr : rule.attributes) {
            oss << "  " << attr.name << ": " << attr.value << ";\n";
        }
        oss << "}\n\n";
    }
    
    return oss.str();
}

std::string StyleProcessor::generate_css_for_element(const std::string& element) const {
    std::ostringstream oss;
    
    for (const auto& rule : rules_) {
        if (rule.selector.find(element) != std::string::npos) {
            oss << rule.selector << " {\n";
            for (const auto& attr : rule.attributes) {
                oss << "  " << attr.name << ": " << attr.value << ";\n";
            }
            oss << "}\n\n";
        }
    }
    
    return oss.str();
}

void StyleProcessor::print_rules() const {
    std::cout << "=== Style Rules ===" << std::endl;
    for (size_t i = 0; i < rules_.size(); ++i) {
        const auto& rule = rules_[i];
        std::cout << "Rule " << i << ": " << rule.selector << std::endl;
        for (const auto& attr : rule.attributes) {
            std::cout << "  " << attr.name << ": " << attr.value << std::endl;
        }
        std::cout << std::endl;
    }
}

void StyleProcessor::print_context() const {
    std::cout << "=== Style Context ===" << std::endl;
    std::cout << "Current element: " << context_.get_current_element() << std::endl;
    std::cout << "Context selector: " << context_.infer_context_selector() << std::endl;
}

void StyleProcessor::clear() {
    rules_.clear();
    reset_context();
}

void StyleProcessor::reset_context() {
    context_ = StyleContext();
}

// 私有方法实现
std::string StyleProcessor::process_selector(const std::string& selector) {
    return context_.resolve_selector(selector);
}

int StyleProcessor::calculate_specificity(const std::string& selector) {
    // 简单的特异性计算
    int specificity = 0;
    
    // ID选择器
    std::regex id_regex(R"(#\w+)");
    std::sregex_iterator id_begin(selector.begin(), selector.end(), id_regex);
    std::sregex_iterator id_end;
    specificity += std::distance(id_begin, id_end) * 100;
    
    // 类选择器
    std::regex class_regex(R"(\.\w+)");
    std::sregex_iterator class_begin(selector.begin(), selector.end(), class_regex);
    std::sregex_iterator class_end;
    specificity += std::distance(class_begin, class_end) * 10;
    
    // 元素选择器
    std::regex element_regex(R"(\b\w+\b)");
    std::sregex_iterator element_begin(selector.begin(), selector.end(), element_regex);
    std::sregex_iterator element_end;
    specificity += std::distance(element_begin, element_end);
    
    return specificity;
}

StyleAttribute StyleProcessor::process_attribute(const std::string& name, const std::string& value) {
    StyleAttribute attr(name, value);
    
    // 检测属性类型
    if (value.find('@') != std::string::npos) {
        attr.type = AttributeType::REFERENCE;
    } else if (value.find('?') != std::string::npos) {
        attr.type = AttributeType::CONDITIONAL;
    } else if (value.find('+') != std::string::npos || 
               value.find('-') != std::string::npos ||
               value.find('*') != std::string::npos ||
               value.find('/') != std::string::npos) {
        attr.type = AttributeType::EXPRESSION;
    }
    
    return attr;
}

std::string StyleProcessor::process_attribute_value(const std::string& value) {
    return ExpressionEvaluator::evaluate_expression(value, context_);
}

void StyleProcessor::infer_context_for_rule(StyleRule& rule) {
    if (rule.selector.empty()) {
        rule.selector = context_.infer_context_selector();
    }
}

void StyleProcessor::auto_add_class_id(StyleRule& rule) {
    // 自动添加类或ID
    // 这里可以实现自动化逻辑
}

} // namespace style
} // namespace chtl