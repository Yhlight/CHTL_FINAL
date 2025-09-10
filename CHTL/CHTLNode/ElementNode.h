#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

struct Attribute {
    std::string name;
    std::string value;
};

class ElementNode : public BaseNode {
public:
    explicit ElementNode(std::string tagName) : m_tagName(std::move(tagName)) {}

    NodeType GetType() const override { return NodeType::Element; }

    const std::string& GetTagName() const { return m_tagName; }
    const std::vector<Attribute>& GetAttributes() const { return m_attributes; }
    const NodeList& GetChildren() const { return m_children; }

    void AddAttribute(const Attribute& attribute) {
        m_attributes.push_back(attribute);
    }

    void AddChild(NodePtr child) {
        m_children.push_back(std::move(child));
    }

private:
    std::string m_tagName;
    std::vector<Attribute> m_attributes;
    NodeList m_children;
};

} // namespace CHTL
