#pragma once

#include "BaseNode.hpp"
#include <string>
#include <map>
#include <vector>

namespace CHTL {

/**
 * @brief 自定义样式组节点
 * 
 * 根据 CHTL.md 文档第 397-488 行实现
 * 支持自定义样式组的特例化操作
 */
class CustomStyleNode : public BaseNode {
public:
    CustomStyleNode(const std::string& name, size_t line = 0, size_t column = 0);
    
    // 获取自定义名称
    const std::string& getCustomName() const { return customName_; }
    
    // 设置自定义名称
    void setCustomName(const std::string& name) { customName_ = name; }
    
    // 添加无值属性
    void addUnvaluedProperty(const std::string& property);
    
    // 获取无值属性列表
    const std::vector<std::string>& getUnvaluedProperties() const { return unvaluedProperties_; }
    
    // 检查是否为无值属性
    bool isUnvaluedProperty(const std::string& property) const;
    
    // 添加 CSS 属性
    void addCSSProperty(const std::string& property, const std::string& value);
    
    // 获取 CSS 属性
    std::string getCSSProperty(const std::string& property) const;
    
    // 检查是否有 CSS 属性
    bool hasCSSProperty(const std::string& property) const;
    
    // 获取所有 CSS 属性
    const std::map<std::string, std::string>& getCSSProperties() const { return cssProperties_; }
    
    // 添加要删除的属性
    void addDeletedProperty(const std::string& property);
    
    // 获取要删除的属性列表
    const std::vector<std::string>& getDeletedProperties() const { return deletedProperties_; }
    
    // 检查是否要删除属性
    bool isDeletedProperty(const std::string& property) const;
    
    // 添加要删除的继承
    void addDeletedInheritance(const std::string& templateName);
    
    // 获取要删除的继承列表
    const std::vector<std::string>& getDeletedInheritances() const { return deletedInheritances_; }
    
    // 检查是否要删除继承
    bool isDeletedInheritance(const std::string& templateName) const;
    
    // 添加继承的模板
    void addInheritedTemplate(const std::string& templateName);
    
    // 获取继承的模板列表
    const std::vector<std::string>& getInheritedTemplates() const { return inheritedTemplates_; }
    
    // 检查是否继承指定模板
    bool inheritsFrom(const std::string& templateName) const;
    
    // 应用特例化操作
    void applySpecialization();
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为 CSS
    std::string toCSS() const;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string customName_;
    std::vector<std::string> unvaluedProperties_;
    std::map<std::string, std::string> cssProperties_;
    std::vector<std::string> deletedProperties_;
    std::vector<std::string> deletedInheritances_;
    std::vector<std::string> inheritedTemplates_;
    
    // 生成 CSS 规则
    std::string generateCSSRules() const;
};

} // namespace CHTL