#include "StyleNode.h"
#include "Visitor.h"

namespace CHTL {

void StyleNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> StyleNode::clone() const {
    auto new_node = std::make_unique<StyleNode>();
    new_node->placeholder_key = this->placeholder_key;
    for (const auto& prop : this->direct_properties) {
        new_node->direct_properties.push_back(prop.clone());
    }
    for (const auto& app : this->template_applications) {
        new_node->template_applications.push_back(app.clone());
    }
    for (const auto& rule : this->global_rules) {
        new_node->global_rules.push_back(rule.clone());
    }
    return new_node;
}

} // namespace CHTL
