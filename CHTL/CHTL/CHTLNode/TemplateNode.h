#pragma once

#include "BaseNode.h"

namespace CHTL {

enum class TemplateType {
    STYLE,      // 样式组模板
    ELEMENT,    // 元素模板
    VAR         // 变量组模板
};

class TemplateNode : public BaseNode {
private:
    TemplateType templateType;
    std::string templateName;
    std::map<std::string, std::string> variables;
    std::vector<std::shared_ptr<BaseNode>> children;
    
public:
    TemplateNode(TemplateType type, const std::string& name);
    virtual ~TemplateNode() = default;
    
    // 模板类型
    TemplateType getTemplateType() const { return templateType; }
    void setTemplateType(TemplateType type) { templateType = type; }
    
    // 模板名称
    const std::string& getTemplateName() const { return templateName; }
    void setTemplateName(const std::string& name) { templateName = name; }
    
    // 变量管理
    void setVariable(const std::string& key, const std::string& value);
    std::string getVariable(const std::string& key) const;
    bool hasVariable(const std::string& key) const;
    void removeVariable(const std::string& key);
    const std::map<std::string, std::string>& getVariables() const { return variables; }
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    std::vector<std::shared_ptr<BaseNode>>& getChildren() { return children; }
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children; }
    
    // 继承管理
    void addInheritance(const std::string& templateName);
    std::vector<std::string> getInheritances() const;
    
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
    
private:
    std::vector<std::string> inheritances;
};

} // namespace CHTL