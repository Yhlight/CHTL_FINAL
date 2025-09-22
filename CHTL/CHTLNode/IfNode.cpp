#include "IfNode.h"

namespace CHTL {

void IfNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> IfNode::clone() const {
    auto new_node = std::make_unique<IfNode>();
    if (condition) {
        new_node->condition = condition->clone();
    }
    for (const auto& child : children) {
        new_node->children.push_back(child->clone());
    }
    for (const auto& prop : properties) {
        new_node->properties.push_back(prop.clone());
    }
    return new_node;
}

} // namespace CHTL
