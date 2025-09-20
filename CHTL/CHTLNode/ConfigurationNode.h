#ifndef CONFIGURATION_NODE_H
#define CONFIGURATION_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <variant>

namespace CHTL {

// Using a variant to hold different types of configuration values.
using ConfigValue = std::variant<int, bool, std::string, std::vector<std::string>>;

class ConfigurationNode : public BaseNode {
public:
    explicit ConfigurationNode(const std::string& name = "") : name(name) {}

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    // The optional name of the configuration block (e.g., "@Config Basic")
    std::string name;

    // Stores simple key-value settings like DEBUG_MODE = false
    std::map<std::string, ConfigValue> settings;

    // Stores the keyword remappings from a nested [Name] block
    std::map<std::string, std::vector<std::string>> name_group;
};

} // namespace CHTL

#endif // CONFIGURATION_NODE_H
