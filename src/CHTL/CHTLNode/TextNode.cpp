#include "CHTL/CHTLNode/TextNode.h"
#include <sstream>

namespace CHTL {

TextNode::TextNode(const std::string& text) 
    : BaseNode(NodeType::TEXT), m_text(text), m_literal(false), m_generatorComment(false) {
}

TextNode::~TextNode() = default;

void TextNode::setText(const std::string& text) {
    m_text = text;
}

std::string TextNode::getText() const {
    return m_text;
}

void TextNode::setLiteral(bool literal) {
    m_literal = literal;
}

bool TextNode::isLiteral() const {
    return m_literal;
}

void TextNode::setGeneratorComment(bool isGeneratorComment) {
    m_generatorComment = isGeneratorComment;
}

bool TextNode::isGeneratorComment() const {
    return m_generatorComment;
}

void TextNode::accept(NodeVisitor& visitor) {
    visitor.visitText(*this);
}

std::shared_ptr<BaseNode> TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(m_text);
    cloned->m_literal = m_literal;
    cloned->m_generatorComment = m_generatorComment;
    cloned->m_content = m_content;
    cloned->m_name = m_name;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    cloned->m_attributes = m_attributes;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool TextNode::validate() const {
    return !m_text.empty() || BaseNode::validate();
}

std::string TextNode::toString() const {
    std::stringstream ss;
    ss << "TextNode{text='" << m_text << "', literal=" << m_literal 
       << ", generatorComment=" << m_generatorComment << "}";
    return ss.str();
}

} // namespace CHTL