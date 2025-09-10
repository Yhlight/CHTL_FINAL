#include "BaseNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

BaseNode::BaseNode(NodeType type, const std::string& name, const std::string& value)
    : nodeType(type), name(name), value(value), line(0), column(0), position(0) {
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string BaseNode::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    return (it != attributes.end()) ? it->second : "";
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

void BaseNode::removeAttribute(const std::string& key) {
    attributes.erase(key);
}

void BaseNode::clearAttributes() {
    attributes.clear();
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children.push_back(child);
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->setParent(nullptr);
        children.erase(it);
    }
}

void BaseNode::insertChild(size_t index, std::shared_ptr<BaseNode> child) {
    if (child && index <= children.size()) {
        child->setParent(shared_from_this());
        children.insert(children.begin() + index, child);
    }
}

std::shared_ptr<BaseNode> BaseNode::getParent() const {
    return parent.lock();
}

void BaseNode::setParent(std::shared_ptr<BaseNode> parent) {
    this->parent = parent;
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildren(const std::string& name) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : children) {
        if (child->getName() == name) {
            result.push_back(child);
        }
    }
    return result;
}

std::shared_ptr<BaseNode> BaseNode::findFirstChild(const std::string& name) const {
    for (const auto& child : children) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

std::shared_ptr<BaseNode> BaseNode::findChildByAttribute(const std::string& attrName, const std::string& attrValue) const {
    for (const auto& child : children) {
        if (child->getAttribute(attrName) == attrValue) {
            return child;
        }
    }
    return nullptr;
}

void BaseNode::traverse(std::function<void(std::shared_ptr<BaseNode>)> callback) {
    callback(shared_from_this());
    for (auto& child : children) {
        child->traverse(callback);
    }
}

void BaseNode::traverse(std::function<void(std::shared_ptr<BaseNode>)> callback) const {
    callback(const_cast<BaseNode*>(this)->shared_from_this());
    for (const auto& child : children) {
        child->traverse(callback);
    }
}

std::shared_ptr<BaseNode> BaseNode::clone() const {
    auto cloned = std::make_shared<BaseNode>(nodeType, name, value);
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool BaseNode::isValid() const {
    return true; // 基础节点总是有效的
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << "BaseNode(" << static_cast<int>(nodeType) << ", \"" << name << "\", \"" << value << "\")";
    return oss.str();
}

std::string BaseNode::toHTML() const {
    return value; // 基础实现，子类可以重写
}

std::string BaseNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << "NodeType: " << static_cast<int>(nodeType) << "\n";
    oss << "Name: " << name << "\n";
    oss << "Value: " << value << "\n";
    oss << "Line: " << line << ", Column: " << column << ", Position: " << position << "\n";
    oss << "Attributes: " << attributes.size() << "\n";
    oss << "Children: " << children.size() << "\n";
    return oss.str();
}

bool BaseNode::operator==(const BaseNode& other) const {
    return nodeType == other.nodeType &&
           name == other.name &&
           value == other.value &&
           attributes == other.attributes &&
           children.size() == other.children.size();
}

bool BaseNode::operator!=(const BaseNode& other) const {
    return !(*this == other);
}

} // namespace CHTL