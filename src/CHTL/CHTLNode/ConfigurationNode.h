#ifndef CHTL_CONFIGURATION_NODE_H
#define CHTL_CONFIGURATION_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

// Using a simple map for now. A more complex structure might be needed
// if configuration values can be more than just simple strings (e.g., arrays for [Name]).
using ConfigValue = std::string;
using ConfigMap = std::map<std::string, ConfigValue>;

class ConfigurationNode : public Node {
public:
    explicit ConfigurationNode(const std::string& name = "") : name_(name) {}

    NodeType getType() const override { return NodeType::Configuration; }

    std::string name_; // For named configuration blocks
    ConfigMap settings_;
    ConfigMap name_group_; // For the [Name] subgroup

    std::unique_ptr<Node> clone() const override {
        auto node = std::make_unique<ConfigurationNode>(name_);
        node->settings_ = settings_;
        node->name_group_ = name_group_;
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_CONFIGURATION_NODE_H
