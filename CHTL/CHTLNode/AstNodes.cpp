#include "ElementNode.h"
#include "TextNode.h"
#include "CommentNode.h"
#include "StyleNode.h"
#include "StyleTemplateNode.h"
#include "ElementTemplateNode.h"
#include "ElementTemplateUsageNode.h"
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

void StyleTemplateNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void ElementTemplateNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

void ElementTemplateUsageNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}
