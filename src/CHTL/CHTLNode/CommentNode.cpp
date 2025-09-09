#include "CHTL/CHTLNode/CommentNode.h"
#include <sstream>

namespace CHTL {

CommentNode::CommentNode(CommentType type, const std::string& content) 
    : BaseNode(NodeType::COMMENT), m_commentType(type), m_commentContent(content) {
}

CommentNode::~CommentNode() = default;

CommentType CommentNode::getCommentType() const {
    return m_commentType;
}

void CommentNode::setCommentContent(const std::string& content) {
    m_commentContent = content;
}

std::string CommentNode::getCommentContent() const {
    return m_commentContent;
}

bool CommentNode::isGeneratorComment() const {
    return m_commentType == CommentType::GENERATOR;
}

void CommentNode::accept(NodeVisitor& visitor) {
    visitor.visitComment(*this);
}

std::shared_ptr<BaseNode> CommentNode::clone() const {
    auto cloned = std::make_shared<CommentNode>(m_commentType, m_commentContent);
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

bool CommentNode::validate() const {
    return BaseNode::validate();
}

std::string CommentNode::toString() const {
    std::stringstream ss;
    ss << "CommentNode{type=" << static_cast<int>(m_commentType) 
       << ", content='" << m_commentContent << "', generatorComment=" << isGeneratorComment() << "}";
    return ss.str();
}

} // namespace CHTL