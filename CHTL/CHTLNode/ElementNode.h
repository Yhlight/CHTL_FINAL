#pragma once

#include "BaseNode.h"
#include "PropertyNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    explicit ElementNode(std::string tagName) : m_tagName(std::move(tagName)) {}

    NodeType GetType() const override { return NodeType::Element; }

    const std::string& GetTagName() const { return m_tagName; }
    std::vector<Property>& GetProperties() { return m_properties; }
    const std::vector<Property>& GetProperties() const { return m_properties; }
    NodeList& GetChildren() { return m_children; }
    const NodeList& GetChildren() const { return m_children; }


    void AddProperty(const Property& property) {
        m_properties.push_back(property);
    }

    void AddChild(NodePtr child) {
        m_children.push_back(std::move(child));
    }

    NodePtr Clone() const override {
        auto clonedNode = std::make_shared<ElementNode>(m_tagName);
        for (const auto& prop : m_properties) {
            // ExpressionNode needs a Clone method too
            clonedNode->AddProperty({prop.name, prop.value ? prop.value->Clone() : nullptr});
        }
        for (const auto& child : m_children) {
            clonedNode->AddChild(child->Clone());
        }
        return clonedNode;
    }

private:
    std::string m_tagName;
    std::vector<Property> m_properties;
    NodeList m_children;
};

} // namespace CHTL
