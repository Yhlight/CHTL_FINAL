#include "NamespaceNode.h"

namespace CHTL {

void NamespaceNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> NamespaceNode::clone() const {
    auto new_node = std::make_unique<NamespaceNode>(name);
    for (const auto& child : children) {
        new_node->addChild(child->clone());
    }
    return new_node;
}

} // namespace CHTL
