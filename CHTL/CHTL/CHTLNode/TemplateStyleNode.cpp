#include "TemplateStyleNode.hpp"
#include <sstream>
#include <algorithm>

namespace CHTL {

TemplateStyleNode::TemplateStyleNode(const std::string& name, size_t line, size_t column)
    : BaseNode(NodeType::TEMPLATE, line, column), templateName_(name) {}

void TemplateStyleNode::addCSSProperty(const std::string& property, const std::string& value) {
    cssProperties_[property] = value;
}

std::string TemplateStyleNode::getCSSProperty(const std::string& property) const {
    auto it = cssProperties_.find(property);
    return (it != cssProperties_.end()) ? it->second : "";
}

bool TemplateStyleNode::hasCSSProperty(const std::string& property) const {
    return cssProperties_.find(property) != cssProperties_.end();
}

void TemplateStyleNode::addInheritedTemplate(const std::string& templateName) {
    inheritedTemplates_.push_back(templateName);
}

bool TemplateStyleNode::inheritsFrom(const std::string& templateName) const {
    return std::find(inheritedTemplates_.begin(), inheritedTemplates_.end(), templateName) 
           != inheritedTemplates_.end();
}

void TemplateStyleNode::mergeInheritedProperties() {
    // 这里需要从全局模板管理器中获取继承的模板属性
    // 暂时简化实现
    for (const auto& inheritedTemplate : inheritedTemplates_) {
        // 在实际实现中，这里会从模板管理器中获取属性并合并
        // 现在只是占位符
    }
}

std::string TemplateStyleNode::toHTML() const {
    // 样式组模板不直接生成 HTML，而是被其他节点引用
    return "";
}

std::string TemplateStyleNode::toCSS() const {
    std::ostringstream oss;
    
    // 生成 CSS 规则
    for (const auto& prop : cssProperties_) {
        oss << prop.first << ": " << prop.second << ";\n";
    }
    
    return oss.str();
}

std::string TemplateStyleNode::toString() const {
    std::ostringstream oss;
    oss << "TemplateStyleNode(name=" << templateName_
        << ", properties=" << cssProperties_.size()
        << ", inherited=" << inheritedTemplates_.size()
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::string TemplateStyleNode::generateCSSRules() const {
    return toCSS();
}

} // namespace CHTL