#include "TemplateNode.h"
#include <sstream>

namespace CHTL {

// TemplateNode implementation
TemplateNode::TemplateNode(NodeType type, const std::string& name)
    : CHTLBaseNode(type), name_(name) {
}

TemplateNode::~TemplateNode() {
}

void TemplateNode::setName(const std::string& name) {
    name_ = name;
}

const std::string& TemplateNode::getName() const {
    return name_;
}

std::string TemplateNode::generateCode() const {
    return "Template: " + name_;
}

std::string TemplateNode::toString() const {
    std::stringstream ss;
    ss << "TemplateNode(" << name_ << ")";
    return ss.str();
}

// TemplateStyleNode implementation
TemplateStyleNode::TemplateStyleNode(const std::string& name)
    : TemplateNode(NodeType::TEMPLATE_STYLE, name) {
}

TemplateStyleNode::~TemplateStyleNode() {
}

void TemplateStyleNode::addStyleProperty(const std::string& property, const std::string& value) {
    style_properties_[property] = value;
}

const std::map<std::string, std::string>& TemplateStyleNode::getStyleProperties() const {
    return style_properties_;
}

std::string TemplateStyleNode::generateCode() const {
    std::stringstream ss;
    ss << "[Template] @Style " << getName() << "\n";
    ss << "{\n";
    
    for (const auto& prop : style_properties_) {
        ss << "    " << prop.first << ": " << prop.second << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

// TemplateElementNode implementation
TemplateElementNode::TemplateElementNode(const std::string& name)
    : TemplateNode(NodeType::TEMPLATE_ELEMENT, name) {
}

TemplateElementNode::~TemplateElementNode() {
}

std::string TemplateElementNode::generateCode() const {
    std::stringstream ss;
    ss << "[Template] @Element " << getName() << "\n";
    ss << "{\n";
    
    for (const auto& child : getChildren()) {
        ss << "    " << child->generateCode() << "\n";
    }
    
    ss << "}";
    return ss.str();
}

// TemplateVarNode implementation
TemplateVarNode::TemplateVarNode(const std::string& name)
    : TemplateNode(NodeType::TEMPLATE_VAR, name) {
}

TemplateVarNode::~TemplateVarNode() {
}

void TemplateVarNode::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

const std::map<std::string, std::string>& TemplateVarNode::getVariables() const {
    return variables_;
}

std::string TemplateVarNode::generateCode() const {
    std::stringstream ss;
    ss << "[Template] @Var " << getName() << "\n";
    ss << "{\n";
    
    for (const auto& var : variables_) {
        ss << "    " << var.first << ": " << var.second << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

// CustomNode implementation
CustomNode::CustomNode(NodeType type, const std::string& name)
    : CHTLBaseNode(type), name_(name) {
}

CustomNode::~CustomNode() {
}

void CustomNode::setName(const std::string& name) {
    name_ = name;
}

const std::string& CustomNode::getName() const {
    return name_;
}

std::string CustomNode::generateCode() const {
    return "Custom: " + name_;
}

std::string CustomNode::toString() const {
    std::stringstream ss;
    ss << "CustomNode(" << name_ << ")";
    return ss.str();
}

// CustomStyleNode implementation
CustomStyleNode::CustomStyleNode(const std::string& name)
    : CustomNode(NodeType::CUSTOM_STYLE, name) {
}

CustomStyleNode::~CustomStyleNode() {
}

void CustomStyleNode::addStyleProperty(const std::string& property, const std::string& value) {
    style_properties_[property] = value;
}

const std::map<std::string, std::string>& CustomStyleNode::getStyleProperties() const {
    return style_properties_;
}

void CustomStyleNode::deleteProperty(const std::string& property) {
    deleted_properties_.push_back(property);
}

void CustomStyleNode::deleteInheritance(const std::string& inheritance) {
    deleted_inheritances_.push_back(inheritance);
}

std::string CustomStyleNode::generateCode() const {
    std::stringstream ss;
    ss << "[Custom] @Style " << getName() << "\n";
    ss << "{\n";
    
    for (const auto& prop : style_properties_) {
        // 检查是否被删除
        if (std::find(deleted_properties_.begin(), deleted_properties_.end(), prop.first) == deleted_properties_.end()) {
            ss << "    " << prop.first << ": " << prop.second << ";\n";
        }
    }
    
    ss << "}";
    return ss.str();
}

// CustomElementNode implementation
CustomElementNode::CustomElementNode(const std::string& name)
    : CustomNode(NodeType::CUSTOM_ELEMENT, name) {
}

CustomElementNode::~CustomElementNode() {
}

void CustomElementNode::insertElement(const std::string& position, std::shared_ptr<CHTLBaseNode> element) {
    insertions_.push_back({position, element});
}

void CustomElementNode::deleteElement(const std::string& selector) {
    deletions_.push_back(selector);
}

void CustomElementNode::deleteElementByIndex(int index) {
    deletions_.push_back("[" + std::to_string(index) + "]");
}

std::string CustomElementNode::generateCode() const {
    std::stringstream ss;
    ss << "[Custom] @Element " << getName() << "\n";
    ss << "{\n";
    
    for (const auto& child : getChildren()) {
        ss << "    " << child->generateCode() << "\n";
    }
    
    // 处理插入操作
    for (const auto& insertion : insertions_) {
        ss << "    insert " << insertion.first << " " << insertion.second->generateCode() << "\n";
    }
    
    // 处理删除操作
    for (const auto& deletion : deletions_) {
        ss << "    delete " << deletion << "\n";
    }
    
    ss << "}";
    return ss.str();
}

// CustomVarNode implementation
CustomVarNode::CustomVarNode(const std::string& name)
    : CustomNode(NodeType::CUSTOM_VAR, name) {
}

CustomVarNode::~CustomVarNode() {
}

void CustomVarNode::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

const std::map<std::string, std::string>& CustomVarNode::getVariables() const {
    return variables_;
}

void CustomVarNode::setVariableValue(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string CustomVarNode::generateCode() const {
    std::stringstream ss;
    ss << "[Custom] @Var " << getName() << "\n";
    ss << "{\n";
    
    for (const auto& var : variables_) {
        ss << "    " << var.first << ": " << var.second << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

} // namespace CHTL