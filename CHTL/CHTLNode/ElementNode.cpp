#include "ElementNode.h"

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName) : m_tagName(tagName) {}

NodeType ElementNode::getType() const {
    return NodeType::Element;
}

std::unique_ptr<BaseNode> ElementNode::clone() const {
    auto new_node = std::make_unique<ElementNode>(m_tagName);
    new_node->m_attributes = this->m_attributes;
    for (const auto& child : m_children) {
        new_node->addChild(child->clone());
    }
    return new_node;
}

const std::string& ElementNode::getTagName() const {
    return m_tagName;
}

void ElementNode::setAttribute(const std::string& key, const std::string& value) {
    m_attributes[key] = value;
}

const std::unordered_map<std::string, std::string>& ElementNode::getAttributes() const {
    return m_attributes;
}

} // namespace CHTL
