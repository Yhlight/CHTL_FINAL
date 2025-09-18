#ifndef STYLE_NODE_H
#define STYLE_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include "CssRuleNode.h"
#include "Visitor.h"
#include <vector>

namespace CHTL {

// Represents a style { ... } block within an element.
class StyleNode : public BaseNode {
public:
    void accept(Visitor& visitor) override;

    // A style block contains a list of simple properties for inline styles.
    std::vector<AttributeNode> inline_properties;

    // It also contains a list of full CSS rules for the global stylesheet.
    std::vector<CssRuleNode> global_rules;
};

} // namespace CHTL

#endif // STYLE_NODE_H
