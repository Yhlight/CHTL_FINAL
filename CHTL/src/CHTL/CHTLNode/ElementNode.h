#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include "StyleBlockNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName) : m_tagName(tagName) {}

    NodeType getType() const override { return NodeType::Element; }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<ElementNode>(m_tagName);
        for (const auto& attr : m_attributes) {
            newNode->addAttribute(std::unique_ptr<AttributeNode>(static_cast<AttributeNode*>(attr->clone().release())));
        }
        for (const auto& child : m_children) {
            newNode->addChild(child->clone());
        }
        if (m_styleBlock) {
            newNode->setStyleBlock(std::unique_ptr<StyleBlockNode>(static_cast<StyleBlockNode*>(m_styleBlock->clone().release())));
        }
        return newNode;
    }

    const std::string& getTagName() const { return m_tagName; }

    void addAttribute(std::unique_ptr<AttributeNode> attr) { m_attributes.push_back(std::move(attr)); }
    void addChild(std::unique_ptr<BaseNode> child) { m_children.push_back(std::move(child)); }
    void setStyleBlock(std::unique_ptr<StyleBlockNode> styleBlock) { m_styleBlock = std::move(styleBlock); }

    const std::vector<std::unique_ptr<AttributeNode>>& getAttributes() const { return m_attributes; }
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return m_children; } // Restore const
    const StyleBlockNode* getStyleBlock() const { return m_styleBlock.get(); }

private:
    std::string m_tagName;
    std::vector<std::unique_ptr<AttributeNode>> m_attributes;
    std::vector<std::unique_ptr<BaseNode>> m_children;
    std::unique_ptr<StyleBlockNode> m_styleBlock;
};

} // namespace CHTL
