#ifndef CHTL_CSS_RULE_NODE_H
#define CHTL_CSS_RULE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include "PropertyValue.h"

namespace CHTL {

class CssRuleNode : public Node {
public:
    explicit CssRuleNode(const std::string& selector) : selector_(selector) {}

    NodeType getType() const override { return NodeType::CssRule; }

    std::unique_ptr<Node> clone() const override {
        auto new_node = std::make_unique<CssRuleNode>(selector_);
        for (const auto& prop : this->properties_) {
            new_node->properties_.emplace_back(prop.name, prop.value->clone());
        }
        return new_node;
    }

    std::string selector_;
    std::vector<PropertyDeclaration> properties_;
};

} // namespace CHTL

#endif // CHTL_CSS_RULE_NODE_H
