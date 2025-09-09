#include "BaseNode.hpp"
#include <sstream>

namespace CHTL {

BaseNode::BaseNode(NodeType type, size_t line, size_t column)
    : type_(type), line_(line), column_(column) {}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        children_.push_back(child);
    }
}

std::shared_ptr<BaseNode> BaseNode::getChild(size_t index) const {
    if (index < children_.size()) {
        return children_[index];
    }
    return nullptr;
}

void BaseNode::setAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

std::string BaseNode::getAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return (it != attributes_.end()) ? it->second : "";
}

bool BaseNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << "BaseNode(type=" << static_cast<int>(type_) 
        << ", line=" << line_ << ", column=" << column_
        << ", children=" << children_.size() << ")";
    return oss.str();
}

} // namespace CHTL