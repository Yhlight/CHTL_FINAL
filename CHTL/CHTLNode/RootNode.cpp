#include "RootNode.h"

namespace CHTL {

void RootNode::addChild(std::unique_ptr<BaseNode> child) {
    children.push_back(std::move(child));
}

void RootNode::accept(Visitor& visitor) {
    // The visitor logic for a root node might be special.
    // For now, we can have it visit each child.
    for (auto& child : children) {
        child->accept(visitor);
    }
}

std::unique_ptr<BaseNode> RootNode::clone() const {
    auto node = std::make_unique<RootNode>();
    for (const auto& child : this->children) {
        node->addChild(child->clone());
    }
    return node;
}

}
