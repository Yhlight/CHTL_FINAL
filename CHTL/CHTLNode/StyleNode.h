#ifndef STYLE_NODE_H
#define STYLE_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include "CssRuleNode.h"
#include "Visitor.h"
#include <vector>

namespace CHTL {

#include <string>

// Represents a style { ... } block within an element.
class StyleNode : public BaseNode {
public:
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    // A style block contains a list of simple properties for inline styles.
    std::vector<AttributeNode> inline_properties;

    // It also contains a list of full CSS rules for the global stylesheet.
    std::vector<CssRuleNode> global_rules;

    // For local style blocks, these store automatically detected classes/ids
    std::string auto_class;
    std::string auto_id;

    // These are populated by the generator to help resolve '&' selectors
    std::string parent_element_id;
    std::string parent_element_class;
};

} // namespace CHTL

#endif // STYLE_NODE_H
