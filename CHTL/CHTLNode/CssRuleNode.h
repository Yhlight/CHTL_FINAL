#ifndef CSS_RULE_NODE_H
#define CSS_RULE_NODE_H

#include "AttributeNode.h"
#include <string>
#include <vector>

namespace CHTL {

struct CssRuleNode {
    std::string selector;
    std::vector<AttributeNode> properties;

    CssRuleNode clone() const {
        CssRuleNode new_rule;
        new_rule.selector = selector;
        for (const auto& prop : properties) {
            new_rule.properties.push_back(prop.clone());
        }
        return new_rule;
    }
};

} // namespace CHTL

#endif // CSS_RULE_NODE_H
