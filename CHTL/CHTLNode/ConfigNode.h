#ifndef CONFIG_NODE_H
#define CONFIG_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

struct ConfigSetting {
    std::string key;
    std::string value; // Value can be complex, but string for now
};

class ConfigNode : public BaseNode {
public:
    // Optional name for the configuration block, e.g., @Config Basic
    std::string name;
    std::vector<ConfigSetting> settings;

    // It can also contain a [Name] block, which is like a sub-node.
    // For now, we'll handle this with a nested structure or a separate node type later.

    void addSetting(const ConfigSetting& setting);
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;
};

}

#endif //CONFIG_NODE_H
