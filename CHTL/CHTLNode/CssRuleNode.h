#ifndef CSS_RULE_NODE_H
#define CSS_RULE_NODE_H

#include "AttributeNode.h"
#include <string>
#include <vector>

namespace CHTL {

// Represents a complete CSS rule, like ".my-class { color: blue; }",
// which is to be placed in a global stylesheet.
class CssRuleNode {
public:
    std::string selector;
    std::vector<AttributeNode> properties;
};

} // namespace CHTL

#endif // CSS_RULE_NODE_H
