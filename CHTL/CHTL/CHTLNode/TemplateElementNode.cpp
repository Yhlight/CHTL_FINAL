#include "TemplateElementNode.hpp"
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
    // 这里需要从全局模板管理器中获取继承的模板内容
    // 暂时简化实现
    for (const auto& inheritedTemplate : inheritedTemplates_) {
        // 在实际实现中，这里会从模板管理器中获取内容并合并
        // 现在只是占位符
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