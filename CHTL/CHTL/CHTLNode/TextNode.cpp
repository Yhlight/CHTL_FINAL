#include "TextNode.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

TextNode::TextNode(const std::string& text) 
    : BaseNode(NodeType::TEXT) {
    setText(text);
}

bool TextNode::isWhitespace() const {
    return std::all_of(m_value.begin(), m_value.end(), ::isspace);
}

std::shared_ptr<BaseNode> TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(m_value);
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    return cloned;
}

std::string TextNode::toString() const {
    return m_value;
}

void TextNode::accept(NodeVisitor& visitor) {
    visitor.visitText(*this);
}

} // namespace CHTL