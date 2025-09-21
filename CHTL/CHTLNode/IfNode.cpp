#include "IfNode.h"

namespace CHTL {

std::unique_ptr<BaseNode> IfNode::clone() const {
    auto new_node = std::make_unique<IfNode>();

    if (condition) {
        new_node->condition = condition->clone();
    }

    for (const auto& child : then_branch) {
        new_node->then_branch.push_back(child->clone());
    }

    if (else_if_branch) {
        new_node->else_if_branch = else_if_branch->clone();
    }

    for (const auto& child : else_branch) {
        new_node->else_branch.push_back(child->clone());
    }

    return new_node;
}

} // namespace CHTL
