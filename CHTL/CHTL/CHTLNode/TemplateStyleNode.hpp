#pragma once

#include "BaseNode.hpp"
#include <string>
#include <map>
#include <vector>

namespace CHTL {

/**
 * @brief 样式组模板节点
 * 
 * 根据 CHTL.md 文档第 263-280 行实现
 * 支持样式组模板的定义和使用
 */
class TemplateStyleNode : public BaseNode {
public:
    TemplateStyleNode(const std::string& name, size_t line = 0, size_t column = 0);
    
    // 获取模板名称
    const std::string& getTemplateName() const { return templateName_; }
    
    // 设置模板名称
    void setTemplateName(const std::string& name) { templateName_ = name; }
    
    // 添加 CSS 属性
    void addCSSProperty(const std::string& property, const std::string& value);
    
    // 获取 CSS 属性
    std::string getCSSProperty(const std::string& property) const;
    
    // 检查是否有 CSS 属性
    bool hasCSSProperty(const std::string& property) const;
    
    // 获取所有 CSS 属性
    const std::map<std::string, std::string>& getCSSProperties() const { return cssProperties_; }
    
    // 添加继承的模板
    void addInheritedTemplate(const std::string& templateName);
    
    // 获取继承的模板列表
    const std::vector<std::string>& getInheritedTemplates() const { return inheritedTemplates_; }
    
    // 检查是否继承指定模板
    bool inheritsFrom(const std::string& templateName) const;
    
    // 合并继承的模板属性
    void mergeInheritedProperties();
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为 CSS
    std::string toCSS() const;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string templateName_;
    std::map<std::string, std::string> cssProperties_;
    std::vector<std::string> inheritedTemplates_;
    
    // 生成 CSS 规则
    std::string generateCSSRules() const;
};

} // namespace CHTL