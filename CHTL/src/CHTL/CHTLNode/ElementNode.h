#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include "StyleBlockNode.h" // Include the new style block node
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents an element in CHTL, such as `div`, `span`, or `body`.
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

    void setStyleBlock(std::unique_ptr<StyleBlockNode> styleBlock) {
        m_styleBlock = std::move(styleBlock);
    }

    const std::vector<std::unique_ptr<AttributeNode>>& getAttributes() const { return m_attributes; }
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return m_children; }
    const StyleBlockNode* getStyleBlock() const { return m_styleBlock.get(); }


private:
    std::string m_tagName;
    std::vector<std::unique_ptr<AttributeNode>> m_attributes;
    std::vector<std::unique_ptr<BaseNode>> m_children;
    std::unique_ptr<StyleBlockNode> m_styleBlock;
};

} // namespace CHTL
