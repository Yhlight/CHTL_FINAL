#include "CHTLJSBaseNode.hpp"
#include <sstream>

namespace CHTL {

CHTLJSBaseNode::CHTLJSBaseNode(NodeType type, size_t line, size_t column)
    : type_(type), line_(line), column_(column) {}

void CHTLJSBaseNode::addChild(std::shared_ptr<CHTLJSBaseNode> child) {
    if (child) {
        children_.push_back(child);
    }
}

void CHTLJSBaseNode::setAttribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

std::string CHTLJSBaseNode::getAttribute(const std::string& key) const {
    auto it = attributes_.find(key);
    return (it != attributes_.end()) ? it->second : "";
}

bool CHTLJSBaseNode::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

std::string CHTLJSBaseNode::toString() const {
    std::ostringstream oss;
    oss << "CHTLJSBaseNode(type=" << static_cast<int>(type_)
        << ", children=" << children_.size()
        << ", line=" << line_ << ", column=" << column_ << ")";
    return oss.str();
}

} // namespace CHTL