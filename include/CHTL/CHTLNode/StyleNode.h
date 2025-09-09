#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 样式类型枚举
 */
enum class StyleType {
    LOCAL,      // 局部样式块
    GLOBAL,     // 全局样式块
    INLINE      // 内联样式
};

/**
 * @brief 样式节点
 * 表示CSS样式定义
 */
class StyleNode : public BaseNode {
public:
    StyleNode(StyleType type = StyleType::LOCAL);
    virtual ~StyleNode();
    
    // 样式类型
    StyleType getStyleType() const;
    
    // CSS属性
    void addProperty(const std::string& property, const std::string& value);
    std::string getProperty(const std::string& property) const;
    bool hasProperty(const std::string& property) const;
    void removeProperty(const std::string& property);
    const std::unordered_map<std::string, std::string>& getProperties() const;
    
    // 选择器管理
    void addSelector(const std::string& selector);
    std::vector<std::string> getSelectors() const;
    bool hasSelector(const std::string& selector) const;
    
    // 类选择器
    void addClassSelector(const std::string& className);
    std::vector<std::string> getClassSelectors() const;
    
    // ID选择器
    void addIDSelector(const std::string& id);
    std::vector<std::string> getIDSelectors() const;
    
    // 伪类选择器
    void addPseudoClassSelector(const std::string& pseudoClass);
    std::vector<std::string> getPseudoClassSelectors() const;
    
    // 伪元素选择器
    void addPseudoElementSelector(const std::string& pseudoElement);
    std::vector<std::string> getPseudoElementSelectors() const;
    
    // 上下文推导（&符号）
    void setContextReference(const std::string& reference);
    std::string getContextReference() const;
    bool hasContextReference() const;
    
    // 属性条件表达式
    void addConditionalProperty(const std::string& property, const std::string& condition, 
                               const std::string& trueValue, const std::string& falseValue);
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> getConditionalProperties() const;
    
    // 链式条件表达式
    void addChainedCondition(const std::string& property, 
                           const std::vector<std::pair<std::string, std::string>>& conditions);
    std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>> getChainedConditions() const;
    
    // 指向属性条件表达式
    void addReferenceCondition(const std::string& property, const std::string& reference, 
                             const std::string& condition, const std::string& trueValue, 
                             const std::string& falseValue);
    std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string>> getReferenceConditions() const;
    
    // 模板引用
    void addTemplateReference(const std::string& templateName);
    std::vector<std::string> getTemplateReferences() const;
    
    // 自定义引用
    void addCustomReference(const std::string& customName);
    std::vector<std::string> getCustomReferences() const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    StyleType m_styleType;
    std::unordered_map<std::string, std::string> m_properties;
    std::vector<std::string> m_selectors;
    std::vector<std::string> m_classSelectors;
    std::vector<std::string> m_idSelectors;
    std::vector<std::string> m_pseudoClassSelectors;
    std::vector<std::string> m_pseudoElementSelectors;
    std::string m_contextReference;
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> m_conditionalProperties;
    std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>> m_chainedConditions;
    std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string>> m_referenceConditions;
    std::vector<std::string> m_templateReferences;
    std::vector<std::string> m_customReferences;
};

} // namespace CHTL