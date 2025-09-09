#include "CommentNode.h"

namespace CHTL {

CommentNode::CommentNode(const std::string& content, bool isGeneratorComment) 
    : BaseNode(NodeType::COMMENT), m_content(content), m_isGeneratorComment(isGeneratorComment) {}

std::unique_ptr<BaseNode> CommentNode::clone() const {
    auto cloned = std::make_unique<CommentNode>(m_content, m_isGeneratorComment);
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    if (m_token) {
        cloned->m_token = std::make_unique<Token>(*m_token);
    }
    
    return cloned;
}

void CommentNode::accept(NodeVisitor* visitor) {
    if (visitor) {
        visitor->visitComment(this);
    }
}

std::string CommentNode::toString() const {
    std::stringstream ss;
    ss << "CommentNode(\"" << m_content << "\"";
    if (m_isGeneratorComment) {
        ss << ", generator";
    }
    ss << ")";
    return ss.str();
}

std::string CommentNode::toHTML() const {
    if (m_isGeneratorComment) {
        // 生成器注释会被包含在HTML中
        return "<!-- " + m_content + " -->";
    }
    // 普通注释不会被包含在HTML中
    return "";
}

} // namespace CHTL