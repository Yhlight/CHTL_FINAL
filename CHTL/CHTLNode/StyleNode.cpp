#include "StyleNode.h"
#include "Visitor.h"

namespace CHTL {

void StyleNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> StyleNode::clone() const {
    auto new_node = std::make_unique<StyleNode>();
    new_node->inline_properties = this->inline_properties;
    new_node->global_rules = this->global_rules;
    return new_node;
}

} // namespace CHTL
