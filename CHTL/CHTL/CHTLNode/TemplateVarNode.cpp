#include "TemplateVarNode.hpp"
#include <CHTL/CHTLManage/TemplateManager.hpp>
#include <sstream>
#include <algorithm>

namespace CHTL {

TemplateVarNode::TemplateVarNode(const std::string& name, size_t line, size_t column)
    : BaseNode(NodeType::TEMPLATE, line, column), templateName_(name) {}

void TemplateVarNode::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string TemplateVarNode::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

bool TemplateVarNode::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void TemplateVarNode::addInheritedTemplate(const std::string& templateName) {
    inheritedTemplates_.push_back(templateName);
}

bool TemplateVarNode::inheritsFrom(const std::string& templateName) const {
    return std::find(inheritedTemplates_.begin(), inheritedTemplates_.end(), templateName) 
           != inheritedTemplates_.end();
}

void TemplateVarNode::mergeInheritedVariables() {
    // 从全局模板管理器中获取继承的模板变量并合并
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
            
            // 递归处理继承的模板
            inheritedTemplate->mergeInheritedVariables();
        }
    }
}

std::string TemplateVarNode::resolveVariableReference(const std::string& reference) const {
    auto [templateName, variableName] = parseVariableReference(reference);
    
    if (templateName == templateName_) {
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

std::string TemplateVarNode::toHTML() const {
    // 变量组模板不直接生成 HTML，而是被其他节点引用
    return "";
}


std::string TemplateVarNode::toString() const {
    std::ostringstream oss;
    oss << "TemplateVarNode(name=" << templateName_
        << ", variables=" << variables_.size()
        << ", inherited=" << inheritedTemplates_.size()
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::pair<std::string, std::string> TemplateVarNode::parseVariableReference(const std::string& reference) const {
    // 解析格式：TemplateName(variableName)
    size_t openParen = reference.find('(');
    size_t closeParen = reference.find(')');
    
    if (openParen != std::string::npos && closeParen != std::string::npos && closeParen > openParen) {
        std::string templateName = reference.substr(0, openParen);
        std::string variableName = reference.substr(openParen + 1, closeParen - openParen - 1);
        return {templateName, variableName};
    }
    
    return {"", reference};
}

} // namespace CHTL