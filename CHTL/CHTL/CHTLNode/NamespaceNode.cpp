#include "NamespaceNode.hpp"
#include <CHTL/CHTLNode/ConstraintNode.hpp>
#include <sstream>

namespace CHTL {

NamespaceNode::NamespaceNode(const std::string& namespaceName, size_t line, size_t column)
    : BaseNode(NodeType::NAMESPACE, line, column), namespaceName_(namespaceName) {}

void NamespaceNode::addConstraint(std::shared_ptr<ConstraintNode> constraint) {
    if (constraint) {
        constraints_.push_back(constraint);
    }
}

const std::vector<std::shared_ptr<ConstraintNode>>& NamespaceNode::getConstraints() const {
    return constraints_;
}

std::string NamespaceNode::toHTML() const {
    // 命名空间节点不直接生成 HTML
    return "";
}

std::string NamespaceNode::toString() const {
    std::ostringstream oss;
    oss << "NamespaceNode(name=" << namespaceName_ 
        << ", line=" << getLine() << ", column=" << getColumn()
        << ", constraints=" << constraints_.size() << ")";
    return oss.str();
}

} // namespace CHTL