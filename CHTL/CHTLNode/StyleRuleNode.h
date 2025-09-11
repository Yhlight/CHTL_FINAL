#pragma once

#include "PropertyNode.h"
#include <string>
#include <vector>

namespace CHTL {

class StyleRuleNode {
public:
    explicit StyleRuleNode(std::string selector) : m_selector(std::move(selector)) {}

    const std::string& GetSelector() const { return m_selector; }
    const std::vector<Property>& GetProperties() const { return m_properties; }

    void AddProperty(Property property) {
        m_properties.push_back(std::move(property));
    }

    std::shared_ptr<StyleRuleNode> Clone() const {
        auto clonedNode = std::make_shared<StyleRuleNode>(m_selector);
        for (const auto& prop : m_properties) {
            clonedNode->AddProperty({prop.name, prop.value ? prop.value->Clone() : nullptr});
        }
        return clonedNode;
    }

private:
    std::string m_selector;
    std::vector<Property> m_properties;
};

} // namespace CHTL
