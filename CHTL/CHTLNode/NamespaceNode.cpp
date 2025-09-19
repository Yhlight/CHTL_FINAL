#include "NamespaceNode.h"

namespace CHTL {

void NamespaceNode::addChild(std::unique_ptr<BaseNode> child) {
    children.push_back(std::move(child));
}

void NamespaceNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> NamespaceNode::clone() const {
    auto node = std::make_unique<NamespaceNode>();
    node->name = this->name;
    for (const auto& child : this->children) {
        node->addChild(child->clone());
    }
    return node;
}

}
