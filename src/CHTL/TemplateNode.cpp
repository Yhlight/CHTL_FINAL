#include "CHTL/TemplateNode.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

// TemplateNode 实现
TemplateNode::TemplateNode(TemplateType type, const std::string& name)
    : CHTLNode(NodeType::TEMPLATE), template_type_(type), template_name_(name) {
}

TemplateType TemplateNode::getTemplateType() const {
    return template_type_;
}

void TemplateNode::setTemplateType(TemplateType type) {
    template_type_ = type;
}

std::string TemplateNode::getTemplateName() const {
    return template_name_;
}

void TemplateNode::setTemplateName(const std::string& name) {
    template_name_ = name;
}

void TemplateNode::addInheritance(const std::string& templateName) {
    inheritances_.push_back(templateName);
}

std::vector<std::string> TemplateNode::getInheritances() const {
    return inheritances_;
}

std::shared_ptr<CHTLNode> TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(template_type_, template_name_);
    
    // 复制继承关系
    for (const auto& inheritance : inheritances_) {
        cloned->addInheritance(inheritance);
    }
    
    // 复制子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    cloned->setContent(content_);
    cloned->setPosition(line_, column_);
    
    return cloned;
}

bool TemplateNode::validate() const {
    // 基本验证：模板名称不能为空
    if (template_name_.empty()) {
        return false;
    }
    
    // 验证子节点
    for (const auto& child : children_) {
        if (!child->validate()) {
            return false;
        }
    }
    
    return true;
}

std::string TemplateNode::generateHTML() const {
    // 模板节点本身不生成HTML，而是被应用到其他节点
    return "";
}

// StyleTemplateNode 实现
StyleTemplateNode::StyleTemplateNode(const std::string& name)
    : TemplateNode(TemplateType::STYLE, name) {
}

void StyleTemplateNode::addCSSProperty(const std::string& property, const std::string& value) {
    css_properties_[property] = value;
}

std::string StyleTemplateNode::getCSSProperty(const std::string& property) const {
    auto it = css_properties_.find(property);
    if (it != css_properties_.end()) {
        return it->second;
    }
    return "";
}

bool StyleTemplateNode::hasCSSProperty(const std::string& property) const {
    return css_properties_.find(property) != css_properties_.end();
}

void StyleTemplateNode::removeCSSProperty(const std::string& property) {
    css_properties_.erase(property);
}

std::unordered_map<std::string, std::string> StyleTemplateNode::getCSSProperties() const {
    return css_properties_;
}

void StyleTemplateNode::addSelector(const std::string& selector) {
    selectors_.push_back(selector);
}

std::vector<std::string> StyleTemplateNode::getSelectors() const {
    return selectors_;
}

void StyleTemplateNode::applyToStyleNode(std::shared_ptr<StyleNode> styleNode) const {
    if (!styleNode) return;
    
    // 应用CSS属性
    for (const auto& prop : css_properties_) {
        styleNode->addCSSProperty(prop.first, prop.second);
    }
    
    // 应用选择器
    for (const auto& selector : selectors_) {
        styleNode->addSelector(selector);
    }
}

std::shared_ptr<CHTLNode> StyleTemplateNode::clone() const {
    auto cloned = std::make_shared<StyleTemplateNode>(template_name_);
    
    // 复制CSS属性
    for (const auto& prop : css_properties_) {
        cloned->addCSSProperty(prop.first, prop.second);
    }
    
    // 复制选择器
    for (const auto& selector : selectors_) {
        cloned->addSelector(selector);
    }
    
    // 复制继承关系
    for (const auto& inheritance : inheritances_) {
        cloned->addInheritance(inheritance);
    }
    
    // 复制子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    cloned->setContent(content_);
    cloned->setPosition(line_, column_);
    
    return cloned;
}

bool StyleTemplateNode::validate() const {
    if (!TemplateNode::validate()) {
        return false;
    }
    
    // 验证CSS属性
    for (const auto& prop : css_properties_) {
        if (prop.first.empty() || prop.second.empty()) {
            return false;
        }
    }
    
    return true;
}

std::string StyleTemplateNode::generateHTML() const {
    std::ostringstream oss;
    
    // 生成CSS规则
    for (const auto& selector : selectors_) {
        oss << selector << " {";
        for (const auto& prop : css_properties_) {
            oss << " " << prop.first << ": " << prop.second << ";";
        }
        oss << " }";
    }
    
    return oss.str();
}

// ElementTemplateNode 实现
ElementTemplateNode::ElementTemplateNode(const std::string& name)
    : TemplateNode(TemplateType::ELEMENT, name) {
}

void ElementTemplateNode::applyToElementNode(std::shared_ptr<ElementNode> elementNode) const {
    if (!elementNode) return;
    
    // 应用子节点
    for (const auto& child : children_) {
        elementNode->addChild(child->clone());
    }
    
    // 应用属性
    for (const auto& attr : attributes_) {
        elementNode->setAttribute(attr.first, attr.second);
    }
}

std::shared_ptr<CHTLNode> ElementTemplateNode::clone() const {
    auto cloned = std::make_shared<ElementTemplateNode>(template_name_);
    
    // 复制属性
    for (const auto& attr : attributes_) {
        cloned->setAttribute(attr.first, attr.second);
    }
    
    // 复制继承关系
    for (const auto& inheritance : inheritances_) {
        cloned->addInheritance(inheritance);
    }
    
    // 复制子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    cloned->setContent(content_);
    cloned->setPosition(line_, column_);
    
    return cloned;
}

bool ElementTemplateNode::validate() const {
    if (!TemplateNode::validate()) {
        return false;
    }
    
    // 验证子节点
    for (const auto& child : children_) {
        if (!child->validate()) {
            return false;
        }
    }
    
    return true;
}

std::string ElementTemplateNode::generateHTML() const {
    std::ostringstream oss;
    
    // 生成子节点HTML
    for (const auto& child : children_) {
        oss << child->generateHTML();
    }
    
    return oss.str();
}

// VarTemplateNode 实现
VarTemplateNode::VarTemplateNode(const std::string& name)
    : TemplateNode(TemplateType::VAR, name) {
}

void VarTemplateNode::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string VarTemplateNode::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    return "";
}

bool VarTemplateNode::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void VarTemplateNode::removeVariable(const std::string& name) {
    variables_.erase(name);
}

std::string VarTemplateNode::applyVariables(const std::string& content) const {
    std::string result = content;
    
    // 替换变量
    for (const auto& var : variables_) {
        std::string placeholder = "{{" + var.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos += var.second.length();
        }
    }
    
    return result;
}

std::shared_ptr<CHTLNode> VarTemplateNode::clone() const {
    auto cloned = std::make_shared<VarTemplateNode>(template_name_);
    
    // 复制变量
    for (const auto& var : variables_) {
        cloned->addVariable(var.first, var.second);
    }
    
    // 复制继承关系
    for (const auto& inheritance : inheritances_) {
        cloned->addInheritance(inheritance);
    }
    
    // 复制子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    cloned->setContent(content_);
    cloned->setPosition(line_, column_);
    
    return cloned;
}

bool VarTemplateNode::validate() const {
    if (!TemplateNode::validate()) {
        return false;
    }
    
    // 验证变量
    for (const auto& var : variables_) {
        if (var.first.empty()) {
            return false;
        }
    }
    
    return true;
}

std::string VarTemplateNode::generateHTML() const {
    // 变量模板不直接生成HTML
    return "";
}

// TemplateManager 实现
TemplateManager::TemplateManager() = default;

TemplateManager::~TemplateManager() = default;

void TemplateManager::registerTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (!templateNode) return;
    
    switch (templateNode->getTemplateType()) {
        case TemplateType::STYLE:
            style_templates_[templateNode->getTemplateName()] = templateNode;
            break;
        case TemplateType::ELEMENT:
            element_templates_[templateNode->getTemplateName()] = templateNode;
            break;
        case TemplateType::VAR:
            var_templates_[templateNode->getTemplateName()] = templateNode;
            break;
    }
}

void TemplateManager::unregisterTemplate(TemplateType type, const std::string& name) {
    switch (type) {
        case TemplateType::STYLE:
            style_templates_.erase(name);
            break;
        case TemplateType::ELEMENT:
            element_templates_.erase(name);
            break;
        case TemplateType::VAR:
            var_templates_.erase(name);
            break;
    }
}

std::shared_ptr<TemplateNode> TemplateManager::getTemplate(TemplateType type, const std::string& name) const {
    switch (type) {
        case TemplateType::STYLE: {
            auto it = style_templates_.find(name);
            if (it != style_templates_.end()) {
                return it->second;
            }
            break;
        }
        case TemplateType::ELEMENT: {
            auto it = element_templates_.find(name);
            if (it != element_templates_.end()) {
                return it->second;
            }
            break;
        }
        case TemplateType::VAR: {
            auto it = var_templates_.find(name);
            if (it != var_templates_.end()) {
                return it->second;
            }
            break;
        }
    }
    return nullptr;
}

bool TemplateManager::hasTemplate(TemplateType type, const std::string& name) const {
    return getTemplate(type, name) != nullptr;
}

void TemplateManager::applyStyleTemplate(const std::string& name, std::shared_ptr<StyleNode> styleNode) const {
    auto templateNode = std::dynamic_pointer_cast<StyleTemplateNode>(getTemplate(TemplateType::STYLE, name));
    if (templateNode) {
        templateNode->applyToStyleNode(styleNode);
    }
}

void TemplateManager::applyElementTemplate(const std::string& name, std::shared_ptr<ElementNode> elementNode) const {
    auto templateNode = std::dynamic_pointer_cast<ElementTemplateNode>(getTemplate(TemplateType::ELEMENT, name));
    if (templateNode) {
        templateNode->applyToElementNode(elementNode);
    }
}

std::string TemplateManager::applyVarTemplate(const std::string& name, const std::string& content) const {
    auto templateNode = std::dynamic_pointer_cast<VarTemplateNode>(getTemplate(TemplateType::VAR, name));
    if (templateNode) {
        return templateNode->applyVariables(content);
    }
    return content;
}

void TemplateManager::resolveInheritance() {
    // 解析样式模板继承
    for (auto& pair : style_templates_) {
        resolveTemplateInheritance(pair.second);
    }
    
    // 解析元素模板继承
    for (auto& pair : element_templates_) {
        resolveTemplateInheritance(pair.second);
    }
    
    // 解析变量模板继承
    for (auto& pair : var_templates_) {
        resolveTemplateInheritance(pair.second);
    }
}

void TemplateManager::clear() {
    style_templates_.clear();
    element_templates_.clear();
    var_templates_.clear();
}

void TemplateManager::resolveTemplateInheritance(std::shared_ptr<TemplateNode> templateNode) {
    if (!templateNode) return;
    
    // 解析继承关系
    for (const auto& inheritance : templateNode->getInheritances()) {
        auto baseTemplate = getTemplate(templateNode->getTemplateType(), inheritance);
        if (baseTemplate) {
            mergeTemplateProperties(baseTemplate, templateNode);
        }
    }
}

void TemplateManager::mergeTemplateProperties(std::shared_ptr<TemplateNode> base, std::shared_ptr<TemplateNode> derived) {
    if (!base || !derived) return;
    
    // 根据模板类型进行不同的合并
    if (base->getTemplateType() == TemplateType::STYLE && derived->getTemplateType() == TemplateType::STYLE) {
        auto baseStyle = std::dynamic_pointer_cast<StyleTemplateNode>(base);
        auto derivedStyle = std::dynamic_pointer_cast<StyleTemplateNode>(derived);
        
        if (baseStyle && derivedStyle) {
            // 合并CSS属性（derived覆盖base）
            auto baseProps = baseStyle->getCSSProperties();
            for (const auto& prop : baseProps) {
                if (!derivedStyle->hasCSSProperty(prop.first)) {
                    derivedStyle->addCSSProperty(prop.first, prop.second);
                }
            }
            
            // 合并选择器
            for (const auto& selector : baseStyle->getSelectors()) {
                derivedStyle->addSelector(selector);
            }
        }
    }
    // 其他类型的模板合并逻辑...
}

} // namespace CHTL