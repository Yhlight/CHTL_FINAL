#include "TextNode.h"
#include "Visitor.h"

TextNode::TextNode(const std::string& text) : text(text) {}

void TextNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

const std::string& TextNode::getText() const {
    return text;
}
