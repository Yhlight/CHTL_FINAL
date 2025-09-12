#ifndef CHTL_CSS_RULE_NODE_H
#define CHTL_CSS_RULE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <utility>

namespace CHTL {

class CssRuleNode : public Node {
public:
    explicit CssRuleNode(const std::string& selector) : selector_(selector) {}

    NodeType getType() const override { return NodeType::CssRule; }

    std::string selector_;
    std::vector<std::pair<std::string, std::string>> properties_;
};

} // namespace CHTL

#endif // CHTL_CSS_RULE_NODE_H
