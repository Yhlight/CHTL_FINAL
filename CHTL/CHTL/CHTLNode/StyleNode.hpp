#pragma once

#include "BaseNode.hpp"
#include <string>
#include <map>

namespace CHTL {

/**
 * @brief 样式节点
 * 
 * 表示局部样式块或全局样式
 */
class StyleNode : public BaseNode {
public:
    enum class StyleType {
        LOCAL,      // 局部样式
        GLOBAL,     // 全局样式
        TEMPLATE    // 模板样式
    };
    
    StyleNode(StyleType type, size_t line = 0, size_t column = 0);
    
    // 获取样式类型
    StyleType getStyleType() const { return styleType_; }
    
    // 设置样式类型
    void setStyleType(StyleType type) { styleType_ = type; }
    
    // 添加 CSS 属性
    void addCSSProperty(const std::string& property, const std::string& value);
    
    // 获取 CSS 属性
    std::string getCSSProperty(const std::string& property) const;
    
    // 检查是否有 CSS 属性
    bool hasCSSProperty(const std::string& property) const;
    
    // 获取所有 CSS 属性
    const std::map<std::string, std::string>& getCSSProperties() const { return cssProperties_; }
    
    // 添加选择器
    void addSelector(const std::string& selector);
    
    // 获取选择器列表
    const std::vector<std::string>& getSelectors() const { return selectors_; }
    
    // 检查是否为内联样式
    bool isInlineStyle() const;
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为 CSS
    std::string toCSS() const;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    StyleType styleType_;
    std::map<std::string, std::string> cssProperties_;
    std::vector<std::string> selectors_;
    
    // 生成 CSS 规则
    std::string generateCSSRules() const;
};

} // namespace CHTL