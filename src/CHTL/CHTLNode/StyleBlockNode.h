#ifndef CHTL_STYLE_BLOCK_NODE_H
#define CHTL_STYLE_BLOCK_NODE_H

#include "BaseNode.h"
#include "ExpressionNode.h"
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include "PropertyValue.h"
#include "CssRuleNode.h"

namespace CHTL {

class StyleBlockNode : public Node {
public:
    StyleBlockNode() = default;

    NodeType getType() const override { return NodeType::StyleBlock; }

    std::vector<std::pair<std::string, std::vector<PropertyValue>>> inline_properties_;
    std::vector<std::unique_ptr<CssRuleNode>> rules_;

    std::unique_ptr<Node> clone() const override {
        auto new_node = std::make_unique<StyleBlockNode>();
        for (const auto& prop_pair : this->inline_properties_) {
            std::vector<PropertyValue> new_values;
            for (const auto& value : prop_pair.second) {
                new_values.push_back(std::visit(PropertyValueCloner{}, value));
            }
            new_node->inline_properties_.emplace_back(prop_pair.first, std::move(new_values));
        }
        for (const auto& rule : this->rules_) {
            new_node->rules_.push_back(std::unique_ptr<CssRuleNode>(static_cast<CssRuleNode*>(rule->clone().release())));
        }
        return new_node;
    }
};

} // namespace CHTL

#endif // CHTL_STYLE_BLOCK_NODE_H
