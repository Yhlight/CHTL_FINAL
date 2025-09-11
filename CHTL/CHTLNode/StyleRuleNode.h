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

private:
    std::string m_selector;
    std::vector<Property> m_properties;
};

} // namespace CHTL
