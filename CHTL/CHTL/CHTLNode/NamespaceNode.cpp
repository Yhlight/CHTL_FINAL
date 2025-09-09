#include "NamespaceNode.hpp"
#include <sstream>

namespace CHTL {

NamespaceNode::NamespaceNode(const std::string& namespaceName, size_t line, size_t column)
    : BaseNode(NodeType::NAMESPACE, line, column), namespaceName_(namespaceName) {}

std::string NamespaceNode::toHTML() const {
    // 命名空间节点不直接生成 HTML
    return "";
}

std::string NamespaceNode::toString() const {
    std::ostringstream oss;
    oss << "NamespaceNode(name=" << namespaceName_
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

} // namespace CHTL