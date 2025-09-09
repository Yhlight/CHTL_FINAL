#include "StyleNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

StyleNode::StyleNode(StyleType type, const std::string& selector) 
    : BaseNode(NodeType::STYLE, "style"), styleType(type), selector(selector), 
      useContextDerivation(false) {
}

void StyleNode::setProperty(const std::string& key, const std::string& value) {
    properties[key] = value;
}

std::string StyleNode::getProperty(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second;
    }
    return "";
}

bool StyleNode::hasProperty(const std::string& key) const {
    return properties.find(key) != properties.end();
}

void StyleNode::removeProperty(const std::string& key) {
    properties.erase(key);
}

void StyleNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children.push_back(child);
    }
}

void StyleNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->setParent(nullptr);
        children.erase(it);
    }
}

void StyleNode::setConditionalExpression(const std::string& expression) {
    conditionalExpression = expression;
}

void StyleNode::addChainExpression(const std::string& expression) {
    chainExpressions.push_back(expression);
}

void StyleNode::setAutoClassName(const std::string& className) {
    autoClassName = className;
}

void StyleNode::setAutoId(const std::string& id) {
    autoId = id;
}

std::string StyleNode::getAutoClassName() const {
    return autoClassName;
}

std::string StyleNode::getAutoId() const {
    return autoId;
}

std::shared_ptr<BaseNode> StyleNode::clone() const {
    auto cloned = std::make_shared<StyleNode>(styleType, selector);
    cloned->properties = properties;
    cloned->conditionalExpression = conditionalExpression;
    cloned->chainExpressions = chainExpressions;
    cloned->useContextDerivation = useContextDerivation;
    cloned->contextSelector = contextSelector;
    cloned->autoClassName = autoClassName;
    cloned->autoId = autoId;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string StyleNode::toString() const {
    std::ostringstream oss;
    oss << "StyleNode(" << static_cast<int>(styleType) << ", \"" << selector << "\")";
    return oss.str();
}

std::string StyleNode::toHTML() const {
    // 样式节点不直接生成HTML，但可能生成内联样式
    if (styleType == StyleType::INLINE) {
        return generateInlineCSS(properties);
    }
    return "";
}

std::string StyleNode::toCSS() const {
    return generateCSS(selector, properties);
}

std::string StyleNode::toJS() const {
    // 样式节点通常不生成JS
    return "";
}

bool StyleNode::isValid() const {
    return BaseNode::isValid();
}

std::string StyleNode::debugString() const {
    std::ostringstream oss;
    oss << "StyleNode: " << selector << " (Type: " << static_cast<int>(styleType) 
        << ", Line: " << line << ", Column: " << column << ")";
    return oss.str();
}

std::string StyleNode::generateCSS(const std::string& selector, const std::map<std::string, std::string>& properties) {
    std::ostringstream oss;
    
    if (!selector.empty()) {
        oss << selector << " {\n";
    }
    
    for (const auto& prop : properties) {
        oss << "    " << prop.first << ": " << prop.second << ";\n";
    }
    
    if (!selector.empty()) {
        oss << "}\n";
    }
    
    return oss.str();
}

std::string StyleNode::generateInlineCSS(const std::map<std::string, std::string>& properties) {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& prop : properties) {
        if (!first) {
            oss << "; ";
        }
        oss << prop.first << ": " << prop.second;
        first = false;
    }
    
    return oss.str();
}

} // namespace CHTL