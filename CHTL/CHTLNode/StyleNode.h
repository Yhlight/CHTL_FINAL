#ifndef STYLE_NODE_H
#define STYLE_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include "CssRuleNode.h"
#include "Visitor.h"
#include "AttributeNode.h"
#include <vector>
#include <string>

namespace CHTL {

struct StyleTemplateUsage {
    std::string name;
    std::string from_namespace;
    std::vector<std::string> deleted_properties;
    std::vector<AttributeNode> specialized_properties;
    std::vector<StyleTemplateUsage> deleted_template_usages;

    StyleTemplateUsage clone() const {
        StyleTemplateUsage new_usage;
        new_usage.name = name;
        new_usage.from_namespace = from_namespace;
        new_usage.deleted_properties = deleted_properties;
        for (const auto& prop : specialized_properties) {
            new_usage.specialized_properties.push_back(prop.clone());
        }
        for (const auto& usage : deleted_template_usages) {
            new_usage.deleted_template_usages.push_back(usage.clone());
        }
        return new_usage;
    }
};

// Represents a style { ... } block within an element.
class StyleNode : public BaseNode {
public:
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    // A style block contains a list of simple properties for inline styles.
    std::vector<AttributeNode> inline_properties;

    // It also contains a list of full CSS rules for the global stylesheet.
    std::vector<CssRuleNode> global_rules;

    // Store template usages
    std::vector<StyleTemplateUsage> template_usages;
};

} // namespace CHTL

#endif // STYLE_NODE_H
