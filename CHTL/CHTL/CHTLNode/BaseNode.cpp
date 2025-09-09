#include "BaseNode.h"
#include <algorithm>
#include <sstream>
#include <functional>

namespace CHTL {

BaseNode::BaseNode(NodeType type, const std::string& name) 
    : nodeType(type), name(name), line(0), column(0), position(0) {
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string BaseNode::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    if (it != attributes.end()) {
        return it->second;
    }
    return "";
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

void BaseNode::removeAttribute(const std::string& key) {
    attributes.erase(key);
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

std::shared_ptr<BaseNode> BaseNode::getChild(size_t index) const {
    if (index < children.size()) {
        return children[index];
    }
    return nullptr;
}

std::shared_ptr<BaseNode> BaseNode::getParent() const {
    return parent.lock();
}

void BaseNode::setParent(std::shared_ptr<BaseNode> parent) {
    this->parent = parent;
}

void BaseNode::setPosition(size_t line, size_t column, size_t position) {
    this->line = line;
    this->column = column;
    this->position = position;
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

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildrenByType(NodeType type) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : children) {
        if (child->getNodeType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

void BaseNode::traverse(std::function<void(std::shared_ptr<BaseNode>)> visitor) {
    visitor(shared_from_this());
    for (auto& child : children) {
        child->traverse(visitor);
    }
}

void BaseNode::traverse(std::function<void(std::shared_ptr<BaseNode>)> visitor) const {
    visitor(shared_from_this());
    for (const auto& child : children) {
        child->traverse(visitor);
    }
}

std::shared_ptr<BaseNode> BaseNode::clone() const {
    auto cloned = std::make_shared<BaseNode>(nodeType, name);
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << "BaseNode(" << static_cast<int>(nodeType) << ", \"" << name << "\")";
    return oss.str();
}

std::string BaseNode::toHTML() const {
    return "";
}

std::string BaseNode::toCSS() const {
    return "";
}

std::string BaseNode::toJS() const {
    return "";
}

bool BaseNode::isValid() const {
    return true;
}

void BaseNode::accept(NodeVisitor& visitor) {
    visitor.visit(shared_from_this());
}

void BaseNode::accept(const NodeVisitor& visitor) const {
    visitor.visit(shared_from_this());
}

std::string BaseNode::debugString() const {
    std::ostringstream oss;
    oss << "Node: " << name << " (Type: " << static_cast<int>(nodeType) 
        << ", Line: " << line << ", Column: " << column << ")";
    return oss.str();
}

} // namespace CHTL