#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 模板类型枚举
 */
enum class TemplateType {
    STYLE,      // 样式组模板
    ELEMENT,    // 元素模板
    VAR         // 变量组模板
};

/**
 * @brief 模板节点
 * 表示CHTL模板定义
 */
class TemplateNode : public BaseNode {
public:
    TemplateNode(TemplateType type, const std::string& name);
    virtual ~TemplateNode();
    
    // 模板类型
    TemplateType getTemplateType() const;
    
    // 模板名称
    void setTemplateName(const std::string& name);
    std::string getTemplateName() const;
    
    // 继承管理
    void addInheritance(const std::string& templateName);
    std::vector<std::string> getInheritances() const;
    bool hasInheritance(const std::string& templateName) const;
    
    // 显式继承
    void setExplicitInheritance(const std::string& templateName);
    std::string getExplicitInheritance() const;
    bool hasExplicitInheritance() const;
    
    // 变量定义（用于变量组模板）
    void addVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getVariables() const;
    
    // 样式属性（用于样式组模板）
    void addStyleProperty(const std::string& property, const std::string& value);
    std::string getStyleProperty(const std::string& property) const;
    bool hasStyleProperty(const std::string& property) const;
    const std::unordered_map<std::string, std::string>& getStyleProperties() const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    TemplateType m_templateType;
    std::string m_templateName;
    std::vector<std::string> m_inheritances;
    std::string m_explicitInheritance;
    std::unordered_map<std::string, std::string> m_variables;
    std::unordered_map<std::string, std::string> m_styleProperties;
};

} // namespace CHTL