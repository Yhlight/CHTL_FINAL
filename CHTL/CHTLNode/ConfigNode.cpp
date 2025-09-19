#include "ConfigNode.h"

namespace CHTL {

void ConfigNode::addSetting(const ConfigSetting& setting) {
    settings.push_back(setting);
}

void ConfigNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> ConfigNode::clone() const {
    auto node = std::make_unique<ConfigNode>();
    node->name = this->name;
    node->settings = this->settings;
    return node;
}

}
