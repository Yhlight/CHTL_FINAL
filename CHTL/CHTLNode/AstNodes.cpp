#include "ElementNode.h"
#include "TextNode.h"
#include "CommentNode.h"
#include "StyleNode.h"
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

void StyleNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}
