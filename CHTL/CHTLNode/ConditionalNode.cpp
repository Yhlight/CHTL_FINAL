#include "ConditionalNode.h"

namespace CHTL {

ConditionalNode::ConditionalNode() : else_branch(nullptr) {}

void ConditionalNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> ConditionalNode::clone() const {
    auto node = std::make_unique<ConditionalNode>();
    for (const auto& prop : this->properties) {
        node->properties.push_back(prop.clone());
    }
    if (this->else_branch) {
        node->else_branch = this->else_branch->clone();
    }
    return node;
}

} // namespace CHTL
