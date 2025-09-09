#include "TextNode.h"
#include "Visitor.h"

TextNode::TextNode(const std::string& text) : text(text) {}

void TextNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> TextNode::clone() const {
    return std::make_unique<TextNode>(text);
}

const std::string& TextNode::getText() const {
    return text;
}
