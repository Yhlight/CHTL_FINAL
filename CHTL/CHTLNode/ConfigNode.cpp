#include "ConfigNode.h"
#include "Visitor.h"

namespace CHTL {

void ConfigNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> ConfigNode::clone() const {
    auto node = std::make_unique<ConfigNode>();
    for (const auto& pair : this->settings) {
        node->settings[pair.first] = pair.second->clone();
    }
    for (const auto& pair : this->name_settings) {
        node->name_settings[pair.first] = pair.second;
    }
    return node;
}

} // namespace CHTL
