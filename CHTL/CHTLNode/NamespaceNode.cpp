#include "NamespaceNode.h"
#include "Visitor.h"

namespace CHTL {

NamespaceNode::NamespaceNode(std::string name) : name(std::move(name)) {}

void NamespaceNode::addChild(std::unique_ptr<BaseNode> child) {
    children.push_back(std::move(child));
}

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
