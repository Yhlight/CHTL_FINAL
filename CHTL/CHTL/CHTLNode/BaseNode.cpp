#include "BaseNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

BaseNode::BaseNode(NodeType type, const std::string& name)
    : type_(type), name_(name), line_(0), column_(0) {
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children_.push_back(child);
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(std::weak_ptr<BaseNode>());
        children_.erase(it);
    }
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

const std::string& BaseNode::getAttribute(const std::string& key) const {
    static const std::string empty;
    auto it = attributes_.find(key);
    return (it != attributes_.end()) ? it->second : empty;
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << "BaseNode(type=" << static_cast<int>(type_) 
        << ", name=\"" << name_ << "\"";
    
    if (!attributes_.empty()) {
        oss << ", attributes={";
        bool first = true;
        for (const auto& attr : attributes_) {
            if (!first) oss << ", ";
            oss << attr.first << "=\"" << attr.second << "\"";
            first = false;
        }
        oss << "}";
    }
    
    if (!children_.empty()) {
        oss << ", children=" << children_.size();
    }
    
    oss << ")";
    return oss.str();
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildren(NodeType type) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : children_) {
        if (child->getType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

std::shared_ptr<BaseNode> BaseNode::findFirstChild(NodeType type) const {
    for (const auto& child : children_) {
        if (child->getType() == type) {
            return child;
        }
    }
    return nullptr;
}

std::shared_ptr<BaseNode> BaseNode::findChildByName(const std::string& name) const {
    for (const auto& child : children_) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

} // namespace CHTL