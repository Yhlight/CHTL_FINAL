#include "TemplateElementNode.hpp"
#include <CHTL/CHTLNode/ElementNode.hpp>
#include <CHTL/CHTLManage/TemplateManager.hpp>
#include <sstream>
#include <algorithm>

namespace CHTL {

TemplateElementNode::TemplateElementNode(const std::string& name, size_t line, size_t column)
    : BaseNode(NodeType::TEMPLATE, line, column), templateName_(name) {}

void TemplateElementNode::addInheritedTemplate(const std::string& templateName) {
    inheritedTemplates_.push_back(templateName);
}

bool TemplateElementNode::inheritsFrom(const std::string& templateName) const {
    return std::find(inheritedTemplates_.begin(), inheritedTemplates_.end(), templateName) 
           != inheritedTemplates_.end();
}

void TemplateElementNode::mergeInheritedContent() {
    // 从全局模板管理器中获取继承的模板内容并合并
    for (const auto& inheritedTemplateName : inheritedTemplates_) {
        auto& templateManager = TemplateManager::getInstance();
        auto inheritedTemplate = templateManager.getElementTemplate(inheritedTemplateName);
        
        if (inheritedTemplate) {
            // 合并继承的子节点
            for (const auto& child : inheritedTemplate->getChildren()) {
                addChild(child);
            }
            
            // 递归处理继承的模板
            inheritedTemplate->mergeInheritedContent();
        }
    }
}

std::string TemplateElementNode::toHTML() const {
    std::ostringstream oss;
    
    // 生成元素内容
    for (const auto& child : getChildren()) {
        if (child) {
            oss << child->toHTML();
        }
    }
    
    return oss.str();
}


std::string TemplateElementNode::toString() const {
    std::ostringstream oss;
    oss << "TemplateElementNode(name=" << templateName_
        << ", children=" << getChildCount()
        << ", inherited=" << inheritedTemplates_.size()
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::string TemplateElementNode::generateElementContent() const {
    return toHTML();
}

} // namespace CHTL