#include "TemplateNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

TemplateNode::TemplateNode(TemplateType type, const std::string& name) 
    : BaseNode(NodeType::TEMPLATE_STYLE, "template"), templateType(type), templateName(name) {
    // 根据模板类型设置正确的NodeType
    switch (type) {
        case TemplateType::STYLE:
            nodeType = NodeType::TEMPLATE_STYLE;
            break;
        case TemplateType::ELEMENT:
            nodeType = NodeType::TEMPLATE_ELEMENT;
            break;
        case TemplateType::VAR:
            nodeType = NodeType::TEMPLATE_VAR;
            break;
    }
}

void TemplateNode::setVariable(const std::string& key, const std::string& value) {
    variables[key] = value;
}

std::string TemplateNode::getVariable(const std::string& key) const {
    auto it = variables.find(key);
    if (it != variables.end()) {
        return it->second;
    }
    return "";
}

bool TemplateNode::hasVariable(const std::string& key) const {
    return variables.find(key) != variables.end();
}

void TemplateNode::removeVariable(const std::string& key) {
    variables.erase(key);
}

void TemplateNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children.push_back(child);
    }
}

void TemplateNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->setParent(nullptr);
        children.erase(it);
    }
}

void TemplateNode::addInheritance(const std::string& templateName) {
    inheritances.push_back(templateName);
}

std::vector<std::string> TemplateNode::getInheritances() const {
    return inheritances;
}

std::shared_ptr<BaseNode> TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(templateType, templateName);
    cloned->variables = variables;
    cloned->inheritances = inheritances;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string TemplateNode::toString() const {
    std::ostringstream oss;
    oss << "TemplateNode(" << static_cast<int>(templateType) << ", \"" << templateName << "\")";
    return oss.str();
}

std::string TemplateNode::toHTML() const {
    std::string result;
    
    if (templateType == TemplateType::ELEMENT) {
        // 元素模板生成HTML
        for (const auto& child : children) {
            result += child->toHTML();
        }
    }
    
    return result;
}

std::string TemplateNode::toCSS() const {
    std::string result;
    
    if (templateType == TemplateType::STYLE) {
        // 样式组模板生成CSS
        std::string selector = "." + templateName;
        result += selector + " {\n";
        
        for (const auto& var : variables) {
            result += "    " + var.first + ": " + var.second + ";\n";
        }
        
        result += "}\n";
    }
    
    return result;
}

std::string TemplateNode::toJS() const {
    // 模板通常不直接生成JS
    return "";
}

bool TemplateNode::isValid() const {
    if (templateName.empty()) {
        return false;
    }
    
    return BaseNode::isValid();
}

std::string TemplateNode::debugString() const {
    std::ostringstream oss;
    oss << "TemplateNode: " << templateName << " (Type: " << static_cast<int>(templateType) 
        << ", Line: " << line << ", Column: " << column << ")";
    return oss.str();
}

} // namespace CHTL