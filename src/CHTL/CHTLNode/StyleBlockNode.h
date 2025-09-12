#ifndef CHTL_STYLE_BLOCK_NODE_H
#define CHTL_STYLE_BLOCK_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <utility> // For std::pair
#include <memory>
#include "../CHTLLexer/Token.h" // For Token
#include "CssRuleNode.h"

namespace CHTL {

class StyleBlockNode : public Node {
public:
    StyleBlockNode() = default;

    NodeType getType() const override { return NodeType::StyleBlock; }

    // For inline styles directly on the parent element
    std::vector<std::pair<std::string, std::vector<Token>>> inline_properties_;

    // For full CSS rules to be extracted to a global style tag
    std::vector<std::unique_ptr<CssRuleNode>> rules_;

    std::unique_ptr<Node> clone() const override {
        auto new_node = std::make_unique<StyleBlockNode>();
        new_node->inline_properties_ = this->inline_properties_;
        for (const auto& rule : this->rules_) {
            // CssRuleNode inherits from Node, but we need to downcast from the result of clone()
            new_node->rules_.push_back(std::unique_ptr<CssRuleNode>(static_cast<CssRuleNode*>(rule->clone().release())));
        }
        return new_node;
    }
};

} // namespace CHTL

#endif // CHTL_STYLE_BLOCK_NODE_H
