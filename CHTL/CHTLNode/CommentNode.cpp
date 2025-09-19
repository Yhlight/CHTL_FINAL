#include "CommentNode.h"

namespace CHTL {

void CommentNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> CommentNode::clone() const {
    auto node = std::make_unique<CommentNode>();
    node->comment = this->comment;
    return node;
}

}
