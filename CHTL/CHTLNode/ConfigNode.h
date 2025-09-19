#ifndef CHTL_CONFIG_NODE_H
#define CHTL_CONFIG_NODE_H

#include "BaseNode.h"
#include <string>
#include <map>
#include <variant>

namespace CHTL {

// This node is currently a placeholder in the AST.
// The parser will directly modify the Configuration object.
class ConfigNode : public BaseNode {
public:
    explicit ConfigNode() {}

    void accept(Visitor& visitor) override {
        // visitor.visit(*this); // Visitor would need to be updated, so we do nothing for now.
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ConfigNode>();
    }
};

} // namespace CHTL

#endif // CHTL_CONFIG_NODE_H
