#pragma once

#include "BaseNode.h"
#include "PropertyNode.h"
#include "StyleRuleNode.h"
#include <vector>
#include <memory>

namespace CHTL {

using StyleRuleNodePtr = std::shared_ptr<StyleRuleNode>;

class StyleNode : public BaseNode {
public:
    StyleNode() = default;

    NodeType GetType() const override { return NodeType::Style; }

    // For inline styles
    void AddProperty(Property property) {
        m_properties.push_back(std::move(property));
    }

    const std::vector<Property>& GetProperties() const {
        return m_properties;
    }

    // For selector-based rules
    void AddRule(StyleRuleNodePtr rule) {
        m_rules.push_back(std::move(rule));
    }

    const std::vector<StyleRuleNodePtr>& GetRules() const {
        return m_rules;
    }

    NodePtr Clone() const override {
        auto clonedNode = std::make_shared<StyleNode>();
        for (const auto& prop : m_properties) {
            clonedNode->AddProperty({prop.name, prop.value ? prop.value->Clone() : nullptr});
        }
        for (const auto& rule : m_rules) {
            // StyleRuleNode is not a BaseNode, so it needs its own Clone method
            clonedNode->AddRule(rule->Clone());
        }
        return clonedNode;
    }

private:
    std::vector<Property> m_properties; // For inline styles
    std::vector<StyleRuleNodePtr> m_rules; // For hoisted styles
};

} // namespace CHTL
