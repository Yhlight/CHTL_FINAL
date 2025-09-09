#pragma once

#include "BaseNode.hpp"
#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 元素模板节点
 * 
 * 根据 CHTL.md 文档第 282-321 行实现
 * 支持元素模板的定义和使用
 */
class TemplateElementNode : public BaseNode {
public:
    TemplateElementNode(const std::string& name, size_t line = 0, size_t column = 0);
    
    // 获取模板名称
    const std::string& getTemplateName() const { return templateName_; }
    
    // 设置模板名称
    void setTemplateName(const std::string& name) { templateName_ = name; }
    
    // 添加继承的模板
    void addInheritedTemplate(const std::string& templateName);
    
    // 获取继承的模板列表
    const std::vector<std::string>& getInheritedTemplates() const { return inheritedTemplates_; }
    
    // 检查是否继承指定模板
    bool inheritsFrom(const std::string& templateName) const;
    
    // 合并继承的模板内容
    void mergeInheritedContent();
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string templateName_;
    std::vector<std::string> inheritedTemplates_;
    
    // 生成元素内容
    std::string generateElementContent() const;
};

} // namespace CHTL