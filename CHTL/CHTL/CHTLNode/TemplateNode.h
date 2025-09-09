#ifndef CHTL_TEMPLATE_NODE_H
#define CHTL_TEMPLATE_NODE_H

#include "BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL {

class TemplateNode : public BaseNode {
private:
    // 模板类型
    enum class TemplateType {
        STYLE,      // 样式组模板 @Style
        ELEMENT,    // 元素模板 @Element
        VARIABLE,   // 变量组模板 @Var
        MIXED       // 混合模板
    };
    
    TemplateType templateType;
    std::string templateName;
    std::map<std::string, std::string> parameters;
    std::vector<std::shared_ptr<BaseNode>> templateContent;
    
    // 模板继承
    std::vector<std::string> parentTemplates;
    std::map<std::string, std::string> inheritedProperties;
    
    // 模板约束
    std::vector<std::string> constraints;
    std::map<std::string, std::string> typeConstraints;
    
    // 模板元数据
    std::map<std::string, std::string> metadata;
    bool isAbstract;
    bool isFinal;
    int priority;

public:
    TemplateNode(const std::string& name, TemplateType type = TemplateType::ELEMENT);
    virtual ~TemplateNode() = default;
    
    // 模板类型
    TemplateType getTemplateType() const { return templateType; }
    void setTemplateType(TemplateType type) { templateType = type; }
    std::string getTemplateTypeName() const;
    
    // 模板名称
    const std::string& getTemplateName() const { return templateName; }
    void setTemplateName(const std::string& name) { templateName = name; }
    
    // 参数管理
    void setParameter(const std::string& name, const std::string& value);
    std::string getParameter(const std::string& name) const;
    bool hasParameter(const std::string& name) const;
    void removeParameter(const std::string& name);
    const std::map<std::string, std::string>& getParameters() const { return parameters; }
    
    // 模板内容
    void addTemplateContent(std::shared_ptr<BaseNode> content);
    void removeTemplateContent(std::shared_ptr<BaseNode> content);
    std::vector<std::shared_ptr<BaseNode>>& getTemplateContent() { return templateContent; }
    const std::vector<std::shared_ptr<BaseNode>>& getTemplateContent() const { return templateContent; }
    
    // 模板继承
    void addParentTemplate(const std::string& parentName);
    void removeParentTemplate(const std::string& parentName);
    const std::vector<std::string>& getParentTemplates() const { return parentTemplates; }
    bool hasParentTemplate(const std::string& parentName) const;
    
    void setInheritedProperty(const std::string& name, const std::string& value);
    std::string getInheritedProperty(const std::string& name) const;
    bool hasInheritedProperty(const std::string& name) const;
    void removeInheritedProperty(const std::string& name);
    const std::map<std::string, std::string>& getInheritedProperties() const { return inheritedProperties; }
    
    // 模板约束
    void addConstraint(const std::string& constraint);
    void removeConstraint(const std::string& constraint);
    const std::vector<std::string>& getConstraints() const { return constraints; }
    bool hasConstraint(const std::string& constraint) const;
    
    void setTypeConstraint(const std::string& type, const std::string& constraint);
    std::string getTypeConstraint(const std::string& type) const;
    bool hasTypeConstraint(const std::string& type) const;
    void removeTypeConstraint(const std::string& type);
    const std::map<std::string, std::string>& getTypeConstraints() const { return typeConstraints; }
    
    // 模板元数据
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    const std::map<std::string, std::string>& getMetadata() const { return metadata; }
    
    // 模板特性
    bool isAbstractTemplate() const { return isAbstract; }
    void setAbstract(bool abstract) { isAbstract = abstract; }
    
    bool isFinalTemplate() const { return isFinal; }
    void setFinal(bool final) { isFinal = final; }
    
    int getPriority() const { return priority; }
    void setPriority(int pri) { priority = pri; }
    
    // 模板实例化
    std::shared_ptr<BaseNode> instantiate(const std::map<std::string, std::string>& args = {}) const;
    std::shared_ptr<BaseNode> instantiateStyleTemplate(const std::map<std::string, std::string>& args = {}) const;
    std::shared_ptr<BaseNode> instantiateElementTemplate(const std::map<std::string, std::string>& args = {}) const;
    std::shared_ptr<BaseNode> instantiateVariableTemplate(const std::map<std::string, std::string>& args = {}) const;
    
    // 模板组合
    std::shared_ptr<TemplateNode> combine(const std::shared_ptr<TemplateNode>& other) const;
    std::shared_ptr<TemplateNode> merge(const std::shared_ptr<TemplateNode>& other) const;
    
    // 模板验证
    bool validateTemplate() const;
    bool validateParameters(const std::map<std::string, std::string>& args) const;
    bool validateConstraints() const;
    
    // 模板转换
    std::string toHTML() const override;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toTemplateString() const;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const TemplateNode& other) const;
    bool operator!=(const TemplateNode& other) const;

private:
    // 辅助方法
    std::string processTemplateString(const std::string& templateStr, const std::map<std::string, std::string>& args) const;
    std::string replacePlaceholders(const std::string& str, const std::map<std::string, std::string>& replacements) const;
    std::vector<std::string> parseTemplateString(const std::string& templateStr) const;
    
    // 模板解析
    std::string parseStyleTemplate(const std::string& templateStr) const;
    std::string parseElementTemplate(const std::string& templateStr) const;
    std::string parseVariableTemplate(const std::string& templateStr) const;
    
    // 继承处理
    void processInheritance();
    void mergeParentProperties();
    void resolveInheritanceConflicts();
    
    // 约束检查
    bool checkTypeConstraint(const std::string& type, const std::string& value) const;
    bool checkGeneralConstraint(const std::string& constraint) const;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_NODE_H