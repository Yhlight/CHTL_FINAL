#include "NamespaceNode.h"
#include "Visitor.h"

namespace CHTL {

NamespaceNode::NamespaceNode(const std::string& name) : name(name) {}

void NamespaceNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> NamespaceNode::clone() const {
    auto node = std::make_unique<NamespaceNode>(name);
    for (const auto& child : children) {
        node->children.push_back(child->clone());
    }
    return node;
}

} // namespace CHTL
