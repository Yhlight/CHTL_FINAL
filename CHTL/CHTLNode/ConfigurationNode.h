#ifndef CONFIGURATION_NODE_H
#define CONFIGURATION_NODE_H

#include "BaseNode.h"
#include <string>
#include <map>
#include <memory>

namespace CHTL {

class ConfigurationNode : public BaseNode {
public:
    std::string name; // For named configuration groups
    std::map<std::string, std::string> settings;
    std::map<std::string, std::vector<std::string>> name_group; // For the [Name] block

    void accept(Visitor& visitor) override {
        // visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ConfigurationNode>(*this);
    }
};

} // namespace CHTL

#endif // CONFIGURATION_NODE_H
