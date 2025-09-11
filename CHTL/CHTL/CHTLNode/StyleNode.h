#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include <vector>
#include <map>

namespace CHTL {

class StyleNode : public BaseNode {
private:
    bool isLocalStyle;  // 是否为局部样式块
    std::map<std::string, std::string> styleProperties;  // 样式属性
    std::vector<std::shared_ptr<BaseNode>> styleRules;   // 样式规则
    
    // 样式规则类型
    enum class RuleType {
        INLINE,         // 内联样式
        CLASS_SELECTOR, // 类选择器
        ID_SELECTOR,    // ID选择器
        PSEUDO_CLASS,   // 伪类选择器
        PSEUDO_ELEMENT, // 伪元素选择器
        CONTEXT_REF     // 上下文引用
    };

public:
    StyleNode(bool local = true);
    virtual ~StyleNode() = default;
    
    // 样式类型
    bool isLocalStyleBlock() const { return isLocalStyle; }
    void setLocalStyle(bool local) { isLocalStyle = local; }
    
    // 样式属性操作
    void setStyleProperty(const std::string& property, const std::string& value);
    std::string getStyleProperty(const std::string& property) const;
    bool hasStyleProperty(const std::string& property) const;
    void removeStyleProperty(const std::string& property);
    const std::map<std::string, std::string>& getStyleProperties() const { return styleProperties; }
    
    // 样式规则操作
    void addStyleRule(std::shared_ptr<BaseNode> rule);
    void removeStyleRule(std::shared_ptr<BaseNode> rule);
    std::vector<std::shared_ptr<BaseNode>>& getStyleRules() { return styleRules; }
    const std::vector<std::shared_ptr<BaseNode>>& getStyleRules() const { return styleRules; }
    
    // 内联样式
    void setInlineStyle(const std::string& style);
    std::string getInlineStyle() const;
    
    // 类选择器
    void addClassSelector(const std::string& className, const std::map<std::string, std::string>& properties);
    void removeClassSelector(const std::string& className);
    bool hasClassSelector(const std::string& className) const;
    
    // ID选择器
    void setIdSelector(const std::string& id, const std::map<std::string, std::string>& properties);
    void removeIdSelector(const std::string& id);
    bool hasIdSelector(const std::string& id) const;
    
    // 伪类选择器
    void addPseudoClassSelector(const std::string& pseudoClass, const std::map<std::string, std::string>& properties);
    void removePseudoClassSelector(const std::string& pseudoClass);
    bool hasPseudoClassSelector(const std::string& pseudoClass) const;
    
    // 伪元素选择器
    void addPseudoElementSelector(const std::string& pseudoElement, const std::map<std::string, std::string>& properties);
    void removePseudoElementSelector(const std::string& pseudoElement);
    bool hasPseudoElementSelector(const std::string& pseudoElement) const;
    
    // 上下文引用
    void addContextReference(const std::string& context, const std::map<std::string, std::string>& properties);
    void removeContextReference(const std::string& context);
    bool hasContextReference(const std::string& context) const;
    
    // 属性运算
    std::string evaluatePropertyExpression(const std::string& expression) const;
    std::string evaluateConditionalExpression(const std::string& expression) const;
    
    // 引用属性
    std::string resolvePropertyReference(const std::string& reference) const;
    
    // 验证
    bool isValid() const override;
    
    // 转换为HTML
    std::string toHTML() const override;
    std::string toCSS() const;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const StyleNode& other) const;
    bool operator!=(const StyleNode& other) const;

private:
    // 辅助方法
    std::string formatCSSProperty(const std::string& property, const std::string& value) const;
    std::string formatCSSRule(const std::string& selector, const std::map<std::string, std::string>& properties) const;
    std::string parseSelector(const std::string& selector) const;
    std::string parsePropertyValue(const std::string& value) const;
    
    // 表达式解析
    std::string parseArithmeticExpression(const std::string& expression) const;
    std::string parseConditionalExpression(const std::string& expression) const;
    std::string parseLogicalExpression(const std::string& expression) const;
    
    // 属性引用解析
    std::string parsePropertyReference(const std::string& reference) const;
    std::string resolveElementProperty(const std::string& selector, const std::string& property) const;
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H