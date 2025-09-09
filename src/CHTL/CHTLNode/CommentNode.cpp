#include "CommentNode.h"
#include "Visitor.h"

CommentNode::CommentNode(const std::string& text) : text(text) {}

void CommentNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> CommentNode::clone() const {
    return std::make_unique<CommentNode>(text);
}

const std::string& CommentNode::getText() const {
    return text;
}
