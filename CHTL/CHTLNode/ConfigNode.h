#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>

// Represents a [Configuration] block in the AST.
// While the configuration is mostly a parse-time concept, having a node
// can be useful for debugging and for named configurations.
class ConfigNode : public BaseNode {
public:
    std::string name; // For named configs like '[Configuration] @Config Basic'
    std::map<std::string, std::string> settings;
    std::map<std::string, std::vector<std::string>> nameGroup; // For the [Name] block

    void accept(AstVisitor& visitor) override {
        // Configurations are handled by the parser/context, not visited by the generator.
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<ConfigNode>();
        node->name = this->name;
        node->settings = this->settings;
        node->nameGroup = this->nameGroup;
        return node;
    }
};
