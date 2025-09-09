#pragma once

#include "BaseNode.hpp"
#include <string>
#include <map>
#include <vector>

namespace CHTL {

/**
 * @brief 变量组模板节点
 * 
 * 根据 CHTL.md 文档第 323-354 行实现
 * 支持变量组模板的定义和使用
 */
class TemplateVarNode : public BaseNode {
public:
    TemplateVarNode(const std::string& name, size_t line = 0, size_t column = 0);
    
    // 获取模板名称
    const std::string& getTemplateName() const { return templateName_; }
    
    // 设置模板名称
    void setTemplateName(const std::string& name) { templateName_ = name; }
    
    // 添加变量
    void addVariable(const std::string& name, const std::string& value);
    
    // 获取变量值
    std::string getVariable(const std::string& name) const;
    
    // 检查是否有变量
    bool hasVariable(const std::string& name) const;
    
    // 获取所有变量
    const std::map<std::string, std::string>& getVariables() const { return variables_; }
    
    // 添加继承的模板
    void addInheritedTemplate(const std::string& templateName);
    
    // 获取继承的模板列表
    const std::vector<std::string>& getInheritedTemplates() const { return inheritedTemplates_; }
    
    // 检查是否继承指定模板
    bool inheritsFrom(const std::string& templateName) const;
    
    // 合并继承的模板变量
    void mergeInheritedVariables();
    
    // 解析变量引用
    std::string resolveVariableReference(const std::string& reference) const;
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string templateName_;
    std::map<std::string, std::string> variables_;
    std::vector<std::string> inheritedTemplates_;
    
    // 解析变量引用格式：TemplateName(variableName)
    std::pair<std::string, std::string> parseVariableReference(const std::string& reference) const;
};

} // namespace CHTL