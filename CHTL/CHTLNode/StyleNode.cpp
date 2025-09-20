#include "StyleNode.h"
#include "Visitor.h"

namespace CHTL {

void StyleNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> StyleNode::clone() const {
    auto new_node = std::make_unique<StyleNode>();
    for (const auto& prop : this->inline_properties) {
        new_node->inline_properties.push_back(prop.clone());
    }
    for (const auto& rule : this->global_rules) {
        new_node->global_rules.push_back(rule.clone());
    }
    return new_node;
}

} // namespace CHTL
