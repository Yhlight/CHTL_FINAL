#pragma once

#include "CHTLJSBaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 增强选择器节点
 * 
 * 表示 {{CSS选择器}} 语法
 */
class EnhancedSelectorNode : public CHTLJSBaseNode {
public:
    enum class SelectorType {
        AUTO,           // {{box}} - 自动推断
        CLASS,          // {{.box}} - 类选择器
        ID,             // {{#box}} - ID选择器
        TAG,            // {{button}} - 标签选择器
        DESCENDANT,     // {{.box button}} - 后代选择器
        INDEXED         // {{button[0]}} - 索引选择器
    };
    
    EnhancedSelectorNode(const std::string& selector, SelectorType type = SelectorType::AUTO,
                        size_t line = 0, size_t column = 0);
    
    // 获取选择器类型
    SelectorType getSelectorType() const { return selectorType_; }
    
    // 设置选择器类型
    void setSelectorType(SelectorType type) { selectorType_ = type; }
    
    // 获取选择器字符串
    const std::string& getSelector() const { return getValue(); }
    
    // 设置选择器字符串
    void setSelector(const std::string& selector) { setValue(selector); }
    
    // 转换为 JavaScript 代码
    std::string toJavaScript() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    SelectorType selectorType_;
    
    // 解析选择器类型
    SelectorType parseSelectorType(const std::string& selector) const;
    
    // 生成对应的JavaScript选择器代码
    std::string generateSelectorCode() const;
};

} // namespace CHTL