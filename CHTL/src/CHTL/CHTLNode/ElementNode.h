#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents an element in CHTL, such as `div`, `span`, or `body`.
// This is a central node type, as it forms the main structure of the document.
// It can contain attributes and other nodes as children.
class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName) : m_tagName(tagName) {}

    NodeType getType() const override { return NodeType::Element; }

    const std::string& getTagName() const { return m_tagName; }

    void addAttribute(std::unique_ptr<AttributeNode> attr) {
        m_attributes.push_back(std::move(attr));
    }

    void addChild(std::unique_ptr<BaseNode> child) {
        m_children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<AttributeNode>>& getAttributes() const { return m_attributes; }
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return m_children; }

private:
    std::string m_tagName;
    std::vector<std::unique_ptr<AttributeNode>> m_attributes;
    std::vector<std::unique_ptr<BaseNode>> m_children;
};

} // namespace CHTL
