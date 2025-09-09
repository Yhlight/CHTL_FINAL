#pragma once

#include "BaseNode.hpp"
#include <string>
#include <map>
#include <vector>

namespace CHTL {

/**
 * @brief 自定义变量组节点
 * 
 * 根据 CHTL.md 文档第 688-730 行实现
 * 支持自定义变量组的特例化操作
 */
class CustomVarNode : public BaseNode {
public:
    CustomVarNode(const std::string& name, size_t line = 0, size_t column = 0);
    
    // 获取自定义名称
    const std::string& getCustomName() const { return customName_; }
    
    // 设置自定义名称
    void setCustomName(const std::string& name) { customName_ = name; }
    
    // 添加变量
    void addVariable(const std::string& name, const std::string& value);
    
    // 获取变量值
    std::string getVariable(const std::string& name) const;
    
    // 检查是否有变量
    bool hasVariable(const std::string& name) const;
    
    // 获取所有变量
    const std::map<std::string, std::string>& getVariables() const { return variables_; }
    
    // 添加特例化变量
    void addSpecializedVariable(const std::string& name, const std::string& value);
    
    // 获取特例化变量
    std::string getSpecializedVariable(const std::string& name) const;
    
    // 检查是否有特例化变量
    bool hasSpecializedVariable(const std::string& name) const;
    
    // 获取所有特例化变量
    const std::map<std::string, std::string>& getSpecializedVariables() const { 
        return specializedVariables_; 
    }
    
    // 添加继承的模板
    void addInheritedTemplate(const std::string& templateName);
    
    // 获取继承的模板列表
    const std::vector<std::string>& getInheritedTemplates() const { return inheritedTemplates_; }
    
    // 检查是否继承指定模板
    bool inheritsFrom(const std::string& templateName) const;
    
    // 应用特例化操作
    void applySpecialization();
    
    // 解析变量引用
    std::string resolveVariableReference(const std::string& reference) const;
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string customName_;
    std::map<std::string, std::string> variables_;
    std::map<std::string, std::string> specializedVariables_;
    std::vector<std::string> inheritedTemplates_;
    
    // 解析变量引用格式：TemplateName(variableName = value)
    std::pair<std::string, std::string> parseVariableReference(const std::string& reference) const;
};

} // namespace CHTL