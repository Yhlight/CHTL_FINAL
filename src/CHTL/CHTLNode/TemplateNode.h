#ifndef TEMPLATE_NODE_H
#define TEMPLATE_NODE_H

#include "BaseNode.h"
#include <string>
#include <unordered_map>

namespace CHTL {

class TemplateNode : public BaseNode {
public:
    TemplateNode(const std::string& name, const std::string& type);
    ~TemplateNode() = default;
    
    // 模板类型
    const std::string& getTemplateType() const { return templateType_; }
    void setTemplateType(const std::string& type) { templateType_ = type; }
    
    // 模板内容
    void setTemplateContent(const std::string& content);
    const std::string& getTemplateContent() const { return templateContent_; }
    
    // 参数管理
    void addParameter(const std::string& name, const std::string& value);
    std::string getParameter(const std::string& name) const;
    bool hasParameter(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getParameters() const { return parameters_; }
    
    // 继承管理
    void setParent(const std::string& parent);
    const std::string& getParent() const { return parent_; }
    bool hasParent() const { return !parent_.empty(); }
    
    // 约束管理
    void addConstraint(const std::string& constraint);
    const std::vector<std::string>& getConstraints() const { return constraints_; }
    
    // 渲染
    std::string render(const std::unordered_map<std::string, std::string>& variables = {}) const;
    
    // 重写基类方法
    std::string generateHTML() const override;
    std::string generateCSS() const override;
    std::string generateJS() const override;
    
private:
    std::string templateType_;
    std::string templateContent_;
    std::unordered_map<std::string, std::string> parameters_;
    std::string parent_;
    std::vector<std::string> constraints_;
    
    // 渲染辅助方法
    std::string processParameters(const std::string& content, const std::unordered_map<std::string, std::string>& variables) const;
    std::string processInheritance(const std::string& content) const;
    std::string processConstraints(const std::string& content) const;
};

} // namespace CHTL

#endif // TEMPLATE_NODE_H