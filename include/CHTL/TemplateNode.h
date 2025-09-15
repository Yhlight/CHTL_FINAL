#pragma once

#include "CHTL/CHTLNode.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

// 使用CHTLNode.h中定义的TemplateType

/**
 * 模板节点基类
 */
class TemplateNode : public CHTLNode {
public:
    TemplateNode(CHTLNode::TemplateType type, const std::string& name);
    
    // 模板类型
    CHTLNode::TemplateType getTemplateType() const;
    void setTemplateType(CHTLNode::TemplateType type);
    
    // 模板名称
    std::string getTemplateName() const;
    void setTemplateName(const std::string& name);
    
    // 继承管理
    void addInheritance(const std::string& templateName);
    std::vector<std::string> getInheritances() const;
    
    // 克隆
    std::shared_ptr<CHTLNode> clone() const override;
    
    // 验证
    bool validate() const override;
    
    // 生成HTML
    std::string generateHTML() const override;

protected:
    TemplateType template_type_;
    std::string template_name_;
    std::vector<std::string> inheritances_;
};

/**
 * 样式组模板节点
 */
class StyleTemplateNode : public TemplateNode {
public:
    StyleTemplateNode(const std::string& name);
    
    // CSS属性管理
    void addCSSProperty(const std::string& property, const std::string& value);
    std::string getCSSProperty(const std::string& property) const;
    bool hasCSSProperty(const std::string& property) const;
    void removeCSSProperty(const std::string& property);
    std::unordered_map<std::string, std::string> getCSSProperties() const;
    
    // 选择器管理
    void addSelector(const std::string& selector);
    std::vector<std::string> getSelectors() const;
    
    // 应用模板到样式节点
    void applyToStyleNode(std::shared_ptr<StyleNode> styleNode) const;
    
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;

private:
    std::unordered_map<std::string, std::string> css_properties_;
    std::vector<std::string> selectors_;
};

/**
 * 元素模板节点
 */
class ElementTemplateNode : public TemplateNode {
public:
    ElementTemplateNode(const std::string& name);
    
    // 应用模板到元素节点
    void applyToElementNode(std::shared_ptr<ElementNode> elementNode) const;
    
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;
};

/**
 * 变量组模板节点
 */
class VarTemplateNode : public TemplateNode {
public:
    VarTemplateNode(const std::string& name);
    
    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void removeVariable(const std::string& name);
    
    // 应用模板变量
    std::string applyVariables(const std::string& content) const;
    
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;

private:
    std::unordered_map<std::string, std::string> variables_;
};

/**
 * 模板管理器
 */
class TemplateManager {
public:
    TemplateManager();
    ~TemplateManager();
    
    // 模板注册
    void registerTemplate(std::shared_ptr<TemplateNode> templateNode);
    void unregisterTemplate(CHTLNode::TemplateType type, const std::string& name);
    
    // 模板查询
    std::shared_ptr<TemplateNode> getTemplate(CHTLNode::TemplateType type, const std::string& name) const;
    bool hasTemplate(CHTLNode::TemplateType type, const std::string& name) const;
    
    // 模板应用
    void applyStyleTemplate(const std::string& name, std::shared_ptr<StyleNode> styleNode) const;
    void applyElementTemplate(const std::string& name, std::shared_ptr<ElementNode> elementNode) const;
    std::string applyVarTemplate(const std::string& name, const std::string& content) const;
    
    // 模板继承
    void resolveInheritance();
    
    // 清理
    void clear();

private:
    std::unordered_map<std::string, std::shared_ptr<TemplateNode>> style_templates_;
    std::unordered_map<std::string, std::shared_ptr<TemplateNode>> element_templates_;
    std::unordered_map<std::string, std::shared_ptr<TemplateNode>> var_templates_;
    
    // 继承解析
    void resolveTemplateInheritance(std::shared_ptr<TemplateNode> templateNode);
    void mergeTemplateProperties(std::shared_ptr<TemplateNode> base, std::shared_ptr<TemplateNode> derived);
};

} // namespace CHTL