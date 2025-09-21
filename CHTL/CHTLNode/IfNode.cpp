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
    for (const auto& attr : body) {
        new_node->body.push_back(attr.clone());
    }
    return new_node;
}

} // namespace CHTL
