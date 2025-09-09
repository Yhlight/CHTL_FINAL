#include "CustomVarNode.hpp"
#include <CHTL/CHTLManage/TemplateManager.hpp>
#include <sstream>
#include <algorithm>

namespace CHTL {

CustomVarNode::CustomVarNode(const std::string& name, size_t line, size_t column)
    : BaseNode(NodeType::CUSTOM, line, column), customName_(name) {}

void CustomVarNode::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string CustomVarNode::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

bool CustomVarNode::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void CustomVarNode::addSpecializedVariable(const std::string& name, const std::string& value) {
    specializedVariables_[name] = value;
}

std::string CustomVarNode::getSpecializedVariable(const std::string& name) const {
    auto it = specializedVariables_.find(name);
    return (it != specializedVariables_.end()) ? it->second : "";
}

bool CustomVarNode::hasSpecializedVariable(const std::string& name) const {
    return specializedVariables_.find(name) != specializedVariables_.end();
}

void CustomVarNode::addInheritedTemplate(const std::string& templateName) {
    inheritedTemplates_.push_back(templateName);
}

bool CustomVarNode::inheritsFrom(const std::string& templateName) const {
    return std::find(inheritedTemplates_.begin(), inheritedTemplates_.end(), templateName) 
           != inheritedTemplates_.end();
}

void CustomVarNode::applySpecialization() {
    // 应用特例化操作
    // 1. 合并继承的模板变量
    for (const auto& inheritedTemplateName : inheritedTemplates_) {
        auto& templateManager = TemplateManager::getInstance();
        auto inheritedTemplate = templateManager.getVarTemplate(inheritedTemplateName);
        
        if (inheritedTemplate) {
            // 合并继承的变量
            for (const auto& var : inheritedTemplate->getVariables()) {
                if (!hasVariable(var.first)) {
                    addVariable(var.first, var.second);
                }
            }
        }
    }
    
    // 2. 应用特例化变量覆盖
    for (const auto& [name, value] : specializedVariables_) {
        variables_[name] = value;
    }
}

std::string CustomVarNode::resolveVariableReference(const std::string& reference) const {
    auto [templateName, variableName] = parseVariableReference(reference);
    
    if (templateName == customName_) {
        // 优先返回特例化变量
        if (hasSpecializedVariable(variableName)) {
            return getSpecializedVariable(variableName);
        }
        return getVariable(variableName);
    }
    
    // 查找其他模板的变量
    auto& templateManager = TemplateManager::getInstance();
    auto otherTemplate = templateManager.getVarTemplate(templateName);
    if (otherTemplate) {
        return otherTemplate->getVariable(variableName);
    }
    
    return reference; // 返回原始引用
}

std::string CustomVarNode::toHTML() const {
    // 自定义变量组不直接生成 HTML，而是被其他节点引用
    return "";
}

std::string CustomVarNode::toString() const {
    std::ostringstream oss;
    oss << "CustomVarNode(name=" << customName_
        << ", variables=" << variables_.size()
        << ", specialized=" << specializedVariables_.size()
        << ", inherited=" << inheritedTemplates_.size()
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::pair<std::string, std::string> CustomVarNode::parseVariableReference(const std::string& reference) const {
    // 解析格式：TemplateName(variableName = value) 或 TemplateName(variableName)
    size_t openParen = reference.find('(');
    size_t closeParen = reference.find(')');
    
    if (openParen != std::string::npos && closeParen != std::string::npos && closeParen > openParen) {
        std::string templateName = reference.substr(0, openParen);
        std::string variablePart = reference.substr(openParen + 1, closeParen - openParen - 1);
        
        // 检查是否有等号（特例化）
        size_t equalPos = variablePart.find('=');
        if (equalPos != std::string::npos) {
            std::string variableName = variablePart.substr(0, equalPos);
            std::string variableValue = variablePart.substr(equalPos + 1);
            return {templateName, variableName};
        } else {
            return {templateName, variablePart};
        }
    }
    
    return {"", reference};
}

} // namespace CHTL