#ifndef CHTL_CSS_RULE_NODE_H
#define CHTL_CSS_RULE_NODE_H

#include "BaseNode.h"
#include "ExpressionNode.h"
#include <string>
#include <vector>
#include <utility>
#include "PropertyValue.h"

namespace CHTL {

class CssRuleNode : public Node {
public:
    explicit CssRuleNode(const std::string& selector) : selector_(selector) {}

    NodeType getType() const override { return NodeType::CssRule; }

    std::unique_ptr<Node> clone() const override {
        auto new_node = std::make_unique<CssRuleNode>(selector_);
        for (const auto& prop_pair : this->properties_) {
            std::vector<PropertyValue> new_values;
            for (const auto& value : prop_pair.second) {
                new_values.push_back(std::visit(PropertyValueCloner{}, value));
            }
            new_node->properties_.emplace_back(prop_pair.first, std::move(new_values));
        }
        return new_node;
    }

    std::string selector_;
    std::vector<std::pair<std::string, std::vector<PropertyValue>>> properties_;
};

} // namespace CHTL

#endif // CHTL_CSS_RULE_NODE_H
