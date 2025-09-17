#include "AST.h"

// Each 'accept' method just calls the corresponding 'visit' method on the visitor.
// This is the core of the Visitor pattern's double dispatch mechanism.

void ProgramNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ElementNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void AttributeNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void TextNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void CommentNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void StringLiteralNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void UnquotedLiteralNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void NumberLiteralNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void StyleBlockNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void StylePropertyNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void StyleRuleNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}
