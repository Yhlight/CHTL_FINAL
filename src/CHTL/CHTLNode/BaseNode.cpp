#include "CHTL/CHTLNode/BaseNode.h"
#include <sstream>

namespace CHTL {

BaseNode::BaseNode(NodeType type, size_t line, size_t column)
    : nodeType_(type), line_(line), column_(column) {
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        children_.push_back(child);
    }
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

std::string BaseNode::getAttribute(const std::string& key) const {
    auto it = attributes_.find(key);
    if (it != attributes_.end()) {
        return it->second;
    }
    return "";
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << "BaseNode(" << static_cast<int>(nodeType_) << ", " 
        << line_ << ":" << column_ << ", children: " << children_.size() 
        << ", attributes: " << attributes_.size() << ")";
    return oss.str();
}

} // namespace CHTL