#include "CommentNode.h"
#include "Visitor.h"

CommentNode::CommentNode(const std::string& text) : text(text) {}

void CommentNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

const std::string& CommentNode::getText() const {
    return text;
}
