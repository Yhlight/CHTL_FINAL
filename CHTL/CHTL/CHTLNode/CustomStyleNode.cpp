#include "CustomStyleNode.hpp"
#include <sstream>
#include <algorithm>

namespace CHTL {

CustomStyleNode::CustomStyleNode(const std::string& name, size_t line, size_t column)
    : BaseNode(NodeType::CUSTOM, line, column), customName_(name) {}

void CustomStyleNode::addUnvaluedProperty(const std::string& property) {
    unvaluedProperties_.push_back(property);
}

bool CustomStyleNode::isUnvaluedProperty(const std::string& property) const {
    return std::find(unvaluedProperties_.begin(), unvaluedProperties_.end(), property) 
           != unvaluedProperties_.end();
}

void CustomStyleNode::addCSSProperty(const std::string& property, const std::string& value) {
    cssProperties_[property] = value;
}

std::string CustomStyleNode::getCSSProperty(const std::string& property) const {
    auto it = cssProperties_.find(property);
    return (it != cssProperties_.end()) ? it->second : "";
}

bool CustomStyleNode::hasCSSProperty(const std::string& property) const {
    return cssProperties_.find(property) != cssProperties_.end();
}

void CustomStyleNode::addDeletedProperty(const std::string& property) {
    deletedProperties_.push_back(property);
}

bool CustomStyleNode::isDeletedProperty(const std::string& property) const {
    return std::find(deletedProperties_.begin(), deletedProperties_.end(), property) 
           != deletedProperties_.end();
}

void CustomStyleNode::addDeletedInheritance(const std::string& templateName) {
    deletedInheritances_.push_back(templateName);
}

bool CustomStyleNode::isDeletedInheritance(const std::string& templateName) const {
    return std::find(deletedInheritances_.begin(), deletedInheritances_.end(), templateName) 
           != deletedInheritances_.end();
}

void CustomStyleNode::addInheritedTemplate(const std::string& templateName) {
    inheritedTemplates_.push_back(templateName);
}

bool CustomStyleNode::inheritsFrom(const std::string& templateName) const {
    return std::find(inheritedTemplates_.begin(), inheritedTemplates_.end(), templateName) 
           != inheritedTemplates_.end();
}

void CustomStyleNode::applySpecialization() {
    // 应用特例化操作
    // 1. 删除指定的属性
    for (const auto& prop : deletedProperties_) {
        cssProperties_.erase(prop);
    }
    
    // 2. 删除指定的继承
    for (const auto& inheritance : deletedInheritances_) {
        inheritedTemplates_.erase(
            std::remove(inheritedTemplates_.begin(), inheritedTemplates_.end(), inheritance),
            inheritedTemplates_.end()
        );
    }
    
    // 3. 合并继承的模板属性
    for (const auto& inheritedTemplate : inheritedTemplates_) {
        // 在实际实现中，这里会从模板管理器中获取属性并合并
        // 现在只是占位符
    }
}

std::string CustomStyleNode::toHTML() const {
    // 自定义样式组不直接生成 HTML，而是被其他节点引用
    return "";
}

std::string CustomStyleNode::toCSS() const {
    std::ostringstream oss;
    
    // 生成 CSS 规则
    for (const auto& prop : cssProperties_) {
        oss << prop.first << ": " << prop.second << ";\n";
    }
    
    return oss.str();
}

std::string CustomStyleNode::toString() const {
    std::ostringstream oss;
    oss << "CustomStyleNode(name=" << customName_
        << ", unvalued=" << unvaluedProperties_.size()
        << ", properties=" << cssProperties_.size()
        << ", deleted=" << deletedProperties_.size()
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::string CustomStyleNode::generateCSSRules() const {
    return toCSS();
}

} // namespace CHTL