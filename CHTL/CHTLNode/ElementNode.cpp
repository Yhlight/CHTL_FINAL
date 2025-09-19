#include "ElementNode.h"

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName) : tagName(tagName) {}

void ElementNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ElementNode::addChild(std::unique_ptr<BaseNode> child) {
    children.push_back(std::move(child));
}

void ElementNode::addAttribute(const HtmlAttribute& attr) {
    attributes.push_back(attr);
}

std::unique_ptr<BaseNode> ElementNode::clone() const {
    auto new_node = std::make_unique<ElementNode>(this->tagName);
    new_node->is_virtual = this->is_virtual;
    new_node->attributes = this->attributes;
    for (const auto& child : this->children) {
        new_node->addChild(child->clone());
    }
    return new_node;
}

} // namespace CHTL
