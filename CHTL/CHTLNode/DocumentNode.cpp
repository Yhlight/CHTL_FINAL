#include "DocumentNode.h"
#include "Visitor.h"

namespace CHTL {

void DocumentNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void DocumentNode::addChild(std::unique_ptr<BaseNode> child) {
    children.push_back(std::move(child));
}

std::unique_ptr<BaseNode> DocumentNode::clone() const {
    auto new_node = std::make_unique<DocumentNode>();
    for (const auto& child : this->children) {
        new_node->addChild(child->clone());
    }
    return new_node;
}

} // namespace CHTL
