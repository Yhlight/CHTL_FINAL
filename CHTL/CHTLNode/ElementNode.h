#pragma once

#include "BaseNode.h"
#include "PropertyNode.h" // Includes ExpressionNode
#include <string>
#include <vector>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    explicit ElementNode(std::string tagName) : m_tagName(std::move(tagName)) {}

    NodeType GetType() const override { return NodeType::Element; }

    const std::string& GetTagName() const { return m_tagName; }
    const std::vector<Property>& GetProperties() const { return m_properties; }
    const NodeList& GetChildren() const { return m_children; }

    void AddProperty(const Property& property) {
        m_properties.push_back(property);
    }

    void AddChild(NodePtr child) {
        m_children.push_back(std::move(child));
    }

private:
    std::string m_tagName;
    std::vector<Property> m_properties;
    NodeList m_children;
};

} // namespace CHTL
