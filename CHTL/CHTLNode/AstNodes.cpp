#include "ElementNode.h"
#include "TextNode.h"
#include "CommentNode.h"
#include "AstVisitor.h"

void ElementNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void TextNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void CommentNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}
