#include "CustomNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CustomNode::CustomNode(CustomType type, const std::string& name) 
    : BaseNode(NodeType::CUSTOM_STYLE, "custom"), customType(type), customName(name) {
    // 根据自定义类型设置正确的NodeType
    switch (type) {
        case CustomType::STYLE:
            nodeType = NodeType::CUSTOM_STYLE;
            break;
        case CustomType::ELEMENT:
            nodeType = NodeType::CUSTOM_ELEMENT;
            break;
        case CustomType::VAR:
            nodeType = NodeType::CUSTOM_VAR;
            break;
    }
}

void CustomNode::setVariable(const std::string& key, const std::string& value) {
    variables[key] = value;
}

std::string CustomNode::getVariable(const std::string& key) const {
    auto it = variables.find(key);
    if (it != variables.end()) {
        return it->second;
    }
    return "";
}

bool CustomNode::hasVariable(const std::string& key) const {
    return variables.find(key) != variables.end();
}

void CustomNode::removeVariable(const std::string& key) {
    variables.erase(key);
}

void CustomNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children.push_back(child);
    }
}

void CustomNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->setParent(nullptr);
        children.erase(it);
    }
}

void CustomNode::deleteProperty(const std::string& property) {
    deletedProperties.push_back(property);
}

void CustomNode::deleteInheritance(const std::string& inheritance) {
    deletedInheritances.push_back(inheritance);
}

void CustomNode::overrideProperty(const std::string& property, const std::string& value) {
    overriddenProperties[property] = value;
}

void CustomNode::addInheritance(const std::string& customName) {
    inheritances.push_back(customName);
}

std::vector<std::string> CustomNode::getInheritances() const {
    return inheritances;
}

std::shared_ptr<BaseNode> CustomNode::getChildByIndex(size_t index) const {
    if (index < children.size()) {
        return children[index];
    }
    return nullptr;
}

void CustomNode::insertChildAt(size_t index, std::shared_ptr<BaseNode> child) {
    if (child && index <= children.size()) {
        child->setParent(shared_from_this());
        children.insert(children.begin() + index, child);
    }
}

void CustomNode::removeChildAt(size_t index) {
    if (index < children.size()) {
        children[index]->setParent(nullptr);
        children.erase(children.begin() + index);
    }
}

std::shared_ptr<BaseNode> CustomNode::clone() const {
    auto cloned = std::make_shared<CustomNode>(customType, customName);
    cloned->variables = variables;
    cloned->inheritances = inheritances;
    cloned->deletedProperties = deletedProperties;
    cloned->deletedInheritances = deletedInheritances;
    cloned->overriddenProperties = overriddenProperties;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string CustomNode::toString() const {
    std::ostringstream oss;
    oss << "CustomNode(" << static_cast<int>(customType) << ", \"" << customName << "\")";
    return oss.str();
}

std::string CustomNode::toHTML() const {
    std::string result;
    
    if (customType == CustomType::ELEMENT) {
        // 自定义元素生成HTML
        for (const auto& child : children) {
            result += child->toHTML();
        }
    }
    
    return result;
}

std::string CustomNode::toCSS() const {
    std::string result;
    
    if (customType == CustomType::STYLE) {
        // 自定义样式组生成CSS
        std::string selector = "." + customName;
        result += selector + " {\n";
        
        for (const auto& var : variables) {
            // 检查是否被删除
            if (std::find(deletedProperties.begin(), deletedProperties.end(), var.first) == deletedProperties.end()) {
                result += "    " + var.first + ": " + var.second + ";\n";
            }
        }
        
        result += "}\n";
    }
    
    return result;
}

std::string CustomNode::toJS() const {
    // 自定义通常不直接生成JS
    return "";
}

bool CustomNode::isValid() const {
    if (customName.empty()) {
        return false;
    }
    
    return BaseNode::isValid();
}

std::string CustomNode::debugString() const {
    std::ostringstream oss;
    oss << "CustomNode: " << customName << " (Type: " << static_cast<int>(customType) 
        << ", Line: " << line << ", Column: " << column << ")";
    return oss.str();
}

} // namespace CHTL