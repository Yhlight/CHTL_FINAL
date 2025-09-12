#ifndef CHTL_CONFIGNODE_H
#define CHTL_CONFIGNODE_H

#include "BaseNode.h"
#include <string>
#include <map>

namespace CHTL {

// Represents a [Configuration] block
class ConfigNode : public BaseNode {
public:
    explicit ConfigNode() = default;

    void accept(NodeVisitor& visitor) override;

private:
    // Will hold configuration key-value pairs
    std::map<std::string, std::string> settings;
};

} // namespace CHTL

#endif // CHTL_CONFIGNODE_H
