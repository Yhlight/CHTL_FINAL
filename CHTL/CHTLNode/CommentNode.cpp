#include "CommentNode.h"
#include "Visitor.h"

namespace CHTL {

CommentNode::CommentNode(const std::string& text) : text(text) {}

void CommentNode::accept(Visitor& visitor) {
    visitor.visitCommentNode(*this);
}

std::unique_ptr<BaseNode> CommentNode::clone() const {
    return std::make_unique<CommentNode>(this->text);
}

} // namespace CHTL
