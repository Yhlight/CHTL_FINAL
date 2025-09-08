#include "TextNode.h"

namespace CHTL {

TextNode::TextNode(const std::string& content) : m_content(content) {}

NodeType TextNode::getType() const {
    return NodeType::Text;
}

std::unique_ptr<BaseNode> TextNode::clone() const {
    return std::make_unique<TextNode>(m_content);
}

const std::string& TextNode::getContent() const {
    return m_content;
}

} // namespace CHTL
