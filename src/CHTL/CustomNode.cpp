#include "CHTL/CustomNode.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

// CustomNode 实现
CustomNode::CustomNode(CustomType type, const std::string& name)
    : CHTLNode(NodeType::CUSTOM), custom_type_(type), custom_name_(name) {
}

CustomType CustomNode::getCustomType() const {
    return custom_type_;
}

void CustomNode::setCustomType(CustomType type) {
    custom_type_ = type;
}

std::string CustomNode::getCustomName() const {
    return custom_name_;
}

void CustomNode::setCustomName(const std::string& name) {
    custom_name_ = name;
}

void CustomNode::addSpecialization(const SpecializationOperation& operation) {
    specializations_.push_back(operation);
}

std::vector<SpecializationOperation> CustomNode::getSpecializations() const {
    return specializations_;
}

void CustomNode::clearSpecializations() {
    specializations_.clear();
}

void CustomNode::addInheritance(const std::string& templateName) {
    inheritances_.push_back(templateName);
}

std::vector<std::string> CustomNode::getInheritances() const {
    return inheritances_;
}

std::shared_ptr<CHTLNode> CustomNode::clone() const {
    auto cloned = std::make_shared<CustomNode>(custom_type_, custom_name_);
    
    // 复制特例化操作
    for (const auto& spec : specializations_) {
        cloned->addSpecialization(spec);
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

bool CustomNode::validate() const {
    // 基本验证：自定义名称不能为空
    if (custom_name_.empty()) {
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

std::string CustomNode::generateHTML() const {
    // 自定义节点本身不生成HTML，而是被应用到其他节点
    return "";
}

// CustomStyleNode 实现
CustomStyleNode::CustomStyleNode(const std::string& name)
    : CustomNode(CustomType::STYLE, name) {
}

void CustomStyleNode::addCSSProperty(const std::string& property, const std::string& value) {
    css_properties_[property] = value;
}

std::string CustomStyleNode::getCSSProperty(const std::string& property) const {
    auto it = css_properties_.find(property);
    if (it != css_properties_.end()) {
        return it->second;
    }
    return "";
}

bool CustomStyleNode::hasCSSProperty(const std::string& property) const {
    return css_properties_.find(property) != css_properties_.end();
}

void CustomStyleNode::removeCSSProperty(const std::string& property) {
    css_properties_.erase(property);
}

void CustomStyleNode::addSelector(const std::string& selector) {
    selectors_.push_back(selector);
}

std::vector<std::string> CustomStyleNode::getSelectors() const {
    return selectors_;
}

void CustomStyleNode::addUnvaluedProperty(const std::string& property) {
    unvalued_properties_.insert(property);
}

std::vector<std::string> CustomStyleNode::getUnvaluedProperties() const {
    return std::vector<std::string>(unvalued_properties_.begin(), unvalued_properties_.end());
}

bool CustomStyleNode::hasUnvaluedProperty(const std::string& property) const {
    return unvalued_properties_.find(property) != unvalued_properties_.end();
}

void CustomStyleNode::applyToStyleNode(std::shared_ptr<StyleNode> styleNode) const {
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

std::shared_ptr<CHTLNode> CustomStyleNode::clone() const {
    auto cloned = std::make_shared<CustomStyleNode>(custom_name_);
    
    // 复制CSS属性
    for (const auto& prop : css_properties_) {
        cloned->addCSSProperty(prop.first, prop.second);
    }
    
    // 复制选择器
    for (const auto& selector : selectors_) {
        cloned->addSelector(selector);
    }
    
    // 复制无值属性
    for (const auto& prop : unvalued_properties_) {
        cloned->addUnvaluedProperty(prop);
    }
    
    // 复制特例化操作
    for (const auto& spec : specializations_) {
        cloned->addSpecialization(spec);
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

bool CustomStyleNode::validate() const {
    if (!CustomNode::validate()) {
        return false;
    }
    
    // 验证CSS属性
    for (const auto& prop : css_properties_) {
        if (prop.first.empty()) {
            return false;
        }
    }
    
    return true;
}

std::string CustomStyleNode::generateHTML() const {
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

// CustomElementNode 实现
CustomElementNode::CustomElementNode(const std::string& name)
    : CustomNode(CustomType::ELEMENT, name) {
}

std::shared_ptr<CHTLNode> CustomElementNode::getElementByIndex(size_t index) const {
    if (index < elements_.size()) {
        return elements_[index];
    }
    return nullptr;
}

void CustomElementNode::setElementByIndex(size_t index, std::shared_ptr<CHTLNode> element) {
    if (index < elements_.size()) {
        elements_[index] = element;
    }
}

void CustomElementNode::insertElement(size_t position, std::shared_ptr<CHTLNode> element) {
    if (position <= elements_.size()) {
        elements_.insert(elements_.begin() + position, element);
    }
}

void CustomElementNode::insertElementAfter(const std::string& selector, std::shared_ptr<CHTLNode> element) {
    size_t index = findElementIndex(selector);
    if (index != std::string::npos) {
        insertElement(index + 1, element);
    }
}

void CustomElementNode::insertElementBefore(const std::string& selector, std::shared_ptr<CHTLNode> element) {
    size_t index = findElementIndex(selector);
    if (index != std::string::npos) {
        insertElement(index, element);
    }
}

void CustomElementNode::insertElementAtTop(std::shared_ptr<CHTLNode> element) {
    insertElement(0, element);
}

void CustomElementNode::insertElementAtBottom(std::shared_ptr<CHTLNode> element) {
    elements_.push_back(element);
}

void CustomElementNode::deleteElement(const std::string& selector) {
    size_t index = findElementIndex(selector);
    if (index != std::string::npos) {
        elements_.erase(elements_.begin() + index);
    }
}

void CustomElementNode::deleteElementByIndex(size_t index) {
    if (index < elements_.size()) {
        elements_.erase(elements_.begin() + index);
    }
}

void CustomElementNode::deleteElementByType(const std::string& elementType) {
    elements_.erase(
        std::remove_if(elements_.begin(), elements_.end(),
            [&elementType](const std::shared_ptr<CHTLNode>& element) {
                if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(element)) {
                    return elementNode->getTagName() == elementType;
                }
                return false;
            }),
        elements_.end()
    );
}

void CustomElementNode::replaceElement(const std::string& selector, std::shared_ptr<CHTLNode> element) {
    size_t index = findElementIndex(selector);
    if (index != std::string::npos) {
        elements_[index] = element;
    }
}

void CustomElementNode::replaceElementByIndex(size_t index, std::shared_ptr<CHTLNode> element) {
    if (index < elements_.size()) {
        elements_[index] = element;
    }
}

void CustomElementNode::applyToElementNode(std::shared_ptr<ElementNode> elementNode) const {
    if (!elementNode) return;
    
    // 应用子元素
    for (const auto& element : elements_) {
        elementNode->addChild(element->clone());
    }
    
    // 应用属性
    for (const auto& attr : attributes_) {
        elementNode->setAttribute(attr.first, attr.second);
    }
}

std::shared_ptr<CHTLNode> CustomElementNode::clone() const {
    auto cloned = std::make_shared<CustomElementNode>(custom_name_);
    
    // 复制元素
    for (const auto& element : elements_) {
        cloned->elements_.push_back(element->clone());
    }
    
    // 复制属性
    for (const auto& attr : attributes_) {
        cloned->setAttribute(attr.first, attr.second);
    }
    
    // 复制特例化操作
    for (const auto& spec : specializations_) {
        cloned->addSpecialization(spec);
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

bool CustomElementNode::validate() const {
    if (!CustomNode::validate()) {
        return false;
    }
    
    // 验证元素
    for (const auto& element : elements_) {
        if (!element->validate()) {
            return false;
        }
    }
    
    return true;
}

std::string CustomElementNode::generateHTML() const {
    std::ostringstream oss;
    
    // 生成元素HTML
    for (const auto& element : elements_) {
        oss << element->generateHTML();
    }
    
    return oss.str();
}

size_t CustomElementNode::findElementIndex(const std::string& selector) const {
    // 简化的选择器查找实现
    for (size_t i = 0; i < elements_.size(); ++i) {
        if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(elements_[i])) {
            if (elementNode->getTagName() == selector) {
                return i;
            }
        }
    }
    return std::string::npos;
}

void CustomElementNode::applySpecializations() {
    // 应用特例化操作
    for (const auto& spec : specializations_) {
        switch (spec.type) {
            case SpecializationType::INSERT_ELEMENT:
                // 插入元素逻辑
                break;
            case SpecializationType::DELETE_ELEMENT:
                // 删除元素逻辑
                break;
            case SpecializationType::REPLACE_ELEMENT:
                // 替换元素逻辑
                break;
            default:
                break;
        }
    }
}

// CustomVarNode 实现
CustomVarNode::CustomVarNode(const std::string& name)
    : CustomNode(CustomType::VAR, name) {
}

void CustomVarNode::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string CustomVarNode::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    return "";
}

bool CustomVarNode::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void CustomVarNode::removeVariable(const std::string& name) {
    variables_.erase(name);
}

void CustomVarNode::specializeVariable(const std::string& name, const std::string& value) {
    specializations_[name] = value;
}

std::string CustomVarNode::applyVariables(const std::string& content) const {
    std::string result = content;
    
    // 应用变量
    for (const auto& var : variables_) {
        std::string placeholder = "{{" + var.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos += var.second.length();
        }
    }
    
    // 应用特例化
    for (const auto& spec : specializations_) {
        std::string placeholder = "{{" + spec.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), spec.second);
            pos += spec.second.length();
        }
    }
    
    return result;
}

std::shared_ptr<CHTLNode> CustomVarNode::clone() const {
    auto cloned = std::make_shared<CustomVarNode>(custom_name_);
    
    // 复制变量
    for (const auto& var : variables_) {
        cloned->addVariable(var.first, var.second);
    }
    
    // 复制特例化
    for (const auto& spec : specializations_) {
        cloned->specializeVariable(spec.first, spec.second);
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

bool CustomVarNode::validate() const {
    if (!CustomNode::validate()) {
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

std::string CustomVarNode::generateHTML() const {
    // 变量自定义不直接生成HTML
    return "";
}

// CustomManager 实现
CustomManager::CustomManager() = default;

CustomManager::~CustomManager() = default;

void CustomManager::registerCustom(std::shared_ptr<CustomNode> customNode) {
    if (!customNode) return;
    
    switch (customNode->getCustomType()) {
        case CustomType::STYLE:
            custom_styles_[customNode->getCustomName()] = customNode;
            break;
        case CustomType::ELEMENT:
            custom_elements_[customNode->getCustomName()] = customNode;
            break;
        case CustomType::VAR:
            custom_vars_[customNode->getCustomName()] = customNode;
            break;
    }
}

void CustomManager::unregisterCustom(CustomType type, const std::string& name) {
    switch (type) {
        case CustomType::STYLE:
            custom_styles_.erase(name);
            break;
        case CustomType::ELEMENT:
            custom_elements_.erase(name);
            break;
        case CustomType::VAR:
            custom_vars_.erase(name);
            break;
    }
}

std::shared_ptr<CustomNode> CustomManager::getCustom(CustomType type, const std::string& name) const {
    switch (type) {
        case CustomType::STYLE: {
            auto it = custom_styles_.find(name);
            if (it != custom_styles_.end()) {
                return it->second;
            }
            break;
        }
        case CustomType::ELEMENT: {
            auto it = custom_elements_.find(name);
            if (it != custom_elements_.end()) {
                return it->second;
            }
            break;
        }
        case CustomType::VAR: {
            auto it = custom_vars_.find(name);
            if (it != custom_vars_.end()) {
                return it->second;
            }
            break;
        }
    }
    return nullptr;
}

bool CustomManager::hasCustom(CustomType type, const std::string& name) const {
    return getCustom(type, name) != nullptr;
}

void CustomManager::applyCustomStyle(const std::string& name, std::shared_ptr<StyleNode> styleNode) const {
    auto customNode = std::dynamic_pointer_cast<CustomStyleNode>(getCustom(CustomType::STYLE, name));
    if (customNode) {
        customNode->applyToStyleNode(styleNode);
    }
}

void CustomManager::applyCustomElement(const std::string& name, std::shared_ptr<ElementNode> elementNode) const {
    auto customNode = std::dynamic_pointer_cast<CustomElementNode>(getCustom(CustomType::ELEMENT, name));
    if (customNode) {
        customNode->applyToElementNode(elementNode);
    }
}

std::string CustomManager::applyCustomVar(const std::string& name, const std::string& content) const {
    auto customNode = std::dynamic_pointer_cast<CustomVarNode>(getCustom(CustomType::VAR, name));
    if (customNode) {
        return customNode->applyVariables(content);
    }
    return content;
}

void CustomManager::processSpecializations() {
    // 处理样式自定义特例化
    for (auto& pair : custom_styles_) {
        auto customStyle = std::dynamic_pointer_cast<CustomStyleNode>(pair.second);
        if (customStyle) {
            processStyleSpecializations(customStyle);
        }
    }
    
    // 处理元素自定义特例化
    for (auto& pair : custom_elements_) {
        auto customElement = std::dynamic_pointer_cast<CustomElementNode>(pair.second);
        if (customElement) {
            processElementSpecializations(customElement);
        }
    }
    
    // 处理变量自定义特例化
    for (auto& pair : custom_vars_) {
        auto customVar = std::dynamic_pointer_cast<CustomVarNode>(pair.second);
        if (customVar) {
            processVarSpecializations(customVar);
        }
    }
}

void CustomManager::clear() {
    custom_styles_.clear();
    custom_elements_.clear();
    custom_vars_.clear();
}

void CustomManager::processStyleSpecializations(std::shared_ptr<CustomStyleNode> customStyle) {
    // 处理样式自定义特例化
    for (const auto& spec : customStyle->getSpecializations()) {
        switch (spec.type) {
            case SpecializationType::DELETE_PROPERTY:
                customStyle->removeCSSProperty(spec.target);
                break;
            case SpecializationType::ADD_PROPERTY:
                customStyle->addCSSProperty(spec.target, spec.value);
                break;
            case SpecializationType::MODIFY_PROPERTY:
                customStyle->addCSSProperty(spec.target, spec.value);
                break;
            default:
                break;
        }
    }
}

void CustomManager::processElementSpecializations(std::shared_ptr<CustomElementNode> customElement) {
    // 处理元素自定义特例化
    customElement->applySpecializations();
}

void CustomManager::processVarSpecializations(std::shared_ptr<CustomVarNode> customVar) {
    // 处理变量自定义特例化
    for (const auto& spec : customVar->getSpecializations()) {
        if (spec.type == SpecializationType::MODIFY_PROPERTY) {
            customVar->specializeVariable(spec.target, spec.value);
        }
    }
}

} // namespace CHTL