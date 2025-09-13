#include "ExpressionNode.h"

namespace CHTL {

std::unique_ptr<Node> ArithmeticNode::clone() const {
    auto new_node = std::make_unique<ArithmeticNode>();
    new_node->op = this->op;

    for (const auto& val : this->left) {
        new_node->left.push_back(std::visit(PropertyValueCloner{}, val));
    }
    for (const auto& val : this->right) {
        new_node->right.push_back(std::visit(PropertyValueCloner{}, val));
    }

    return new_node;
}

std::unique_ptr<Node> ConditionalNode::clone() const {
    auto new_node = std::make_unique<ConditionalNode>();

    for (const auto& val : this->condition) {
        new_node->condition.push_back(std::visit(PropertyValueCloner{}, val));
    }
    for (const auto& val : this->true_branch) {
        new_node->true_branch.push_back(std::visit(PropertyValueCloner{}, val));
    }
    for (const auto& val : this->false_branch) {
        new_node->false_branch.push_back(std::visit(PropertyValueCloner{}, val));
    }

    return new_node;
}

} // namespace CHTL
