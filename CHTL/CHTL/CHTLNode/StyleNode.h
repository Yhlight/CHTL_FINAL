#pragma once

#include "BaseNode.h"

namespace CHTL {

enum class StyleType {
    INLINE,         // 内联样式
    CLASS,          // 类选择器
    ID,             // ID选择器
    PSEUDO_CLASS,   // 伪类选择器
    PSEUDO_ELEMENT, // 伪元素选择器
    CONDITIONAL     // 属性条件表达式
};

class StyleNode : public BaseNode {
private:
    StyleType styleType;
    std::string selector;
    std::map<std::string, std::string> properties;
    std::vector<std::shared_ptr<BaseNode>> children;
    
    // 属性条件表达式
    std::string conditionalExpression;
    std::vector<std::string> chainExpressions;
    
    // 上下文推导
    bool useContextDerivation;
    std::string contextSelector;
    
public:
    StyleNode(StyleType type = StyleType::INLINE, const std::string& selector = "");
    virtual ~StyleNode() = default;
    
    // 样式类型
    StyleType getStyleType() const { return styleType; }
    void setStyleType(StyleType type) { styleType = type; }
    
    // 选择器
    const std::string& getSelector() const { return selector; }
    void setSelector(const std::string& selector) { this->selector = selector; }
    
    // 属性管理
    void setProperty(const std::string& key, const std::string& value);
    std::string getProperty(const std::string& key) const;
    bool hasProperty(const std::string& key) const;
    void removeProperty(const std::string& key);
    const std::map<std::string, std::string>& getProperties() const { return properties; }
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    std::vector<std::shared_ptr<BaseNode>>& getChildren() { return children; }
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children; }
    
    // 属性条件表达式
    void setConditionalExpression(const std::string& expression);
    const std::string& getConditionalExpression() const { return conditionalExpression; }
    
    void addChainExpression(const std::string& expression);
    const std::vector<std::string>& getChainExpressions() const { return chainExpressions; }
    
    // 上下文推导
    void setUseContextDerivation(bool use) { useContextDerivation = use; }
    bool isUseContextDerivation() const { return useContextDerivation; }
    
    void setContextSelector(const std::string& selector) { contextSelector = selector; }
    const std::string& getContextSelector() const { return contextSelector; }
    
    // 自动化类名/ID
    void setAutoClassName(const std::string& className);
    void setAutoId(const std::string& id);
    std::string getAutoClassName() const;
    std::string getAutoId() const;
    
    // 克隆
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 序列化
    virtual std::string toString() const override;
    virtual std::string toHTML() const override;
    virtual std::string toCSS() const override;
    virtual std::string toJS() const override;
    
    // 验证
    virtual bool isValid() const override;
    
    // 调试
    virtual std::string debugString() const override;
    
    // 静态方法
    static std::string generateCSS(const std::string& selector, const std::map<std::string, std::string>& properties);
    static std::string generateInlineCSS(const std::map<std::string, std::string>& properties);
    
private:
    std::string autoClassName;
    std::string autoId;
};

} // namespace CHTL