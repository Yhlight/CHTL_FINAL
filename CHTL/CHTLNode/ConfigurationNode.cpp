#include "ConfigurationNode.h"

namespace CHTL {

void ConfigurationNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> ConfigurationNode::clone() const {
    auto new_node = std::make_unique<ConfigurationNode>(name);
    new_node->settings = this->settings;
    new_node->name_group = this->name_group;
    return new_node;
}

} // namespace CHTL
