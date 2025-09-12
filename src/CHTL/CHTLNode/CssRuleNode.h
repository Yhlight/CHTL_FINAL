#ifndef CHTL_CSS_RULE_NODE_H
#define CHTL_CSS_RULE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <utility>
#include "../CHTLLexer/Token.h" // For Token

namespace CHTL {

class CssRuleNode : public Node {
public:
    explicit CssRuleNode(const std::string& selector) : selector_(selector) {}

    NodeType getType() const override { return NodeType::CssRule; }

    std::unique_ptr<Node> clone() const override {
        auto new_node = std::make_unique<CssRuleNode>(selector_);
        new_node->properties_ = this->properties_; // Deep copy of vector of pairs of string and vector
        return new_node;
    }

    std::string selector_;
    std::vector<std::pair<std::string, std::vector<Token>>> properties_;
};

} // namespace CHTL

#endif // CHTL_CSS_RULE_NODE_H
