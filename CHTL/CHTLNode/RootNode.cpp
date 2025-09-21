#include "RootNode.h"
#include "Visitor.h"

namespace CHTL {

void RootNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void RootNode::addChild(std::unique_ptr<BaseNode> child) {
    children.push_back(std::move(child));
}

std::unique_ptr<BaseNode> RootNode::clone() const {
    auto new_node = std::make_unique<RootNode>();
    for (const auto& child : children) {
        new_node->addChild(child->clone());
    }
    return new_node;
}

} // namespace CHTL
