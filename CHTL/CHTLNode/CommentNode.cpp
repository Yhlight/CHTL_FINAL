#include "CommentNode.h"

namespace CHTL {

CommentNode::CommentNode(const std::string& content, bool isGeneratorComment)
    : m_content(content), m_isGeneratorComment(isGeneratorComment) {}

NodeType CommentNode::getType() const {
    return NodeType::Comment;
}

std::unique_ptr<BaseNode> CommentNode::clone() const {
    return std::make_unique<CommentNode>(m_content, m_isGeneratorComment);
}

const std::string& CommentNode::getContent() const {
    return m_content;
}

bool CommentNode::isGeneratorComment() const {
    return m_isGeneratorComment;
}

} // namespace CHTL
