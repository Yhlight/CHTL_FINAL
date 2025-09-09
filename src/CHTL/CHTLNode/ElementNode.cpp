#include "ElementNode.h"
#include "Visitor.h"

ElementNode::ElementNode(const std::string& tagName) : tagName(tagName) {}

void ElementNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

const std::string& ElementNode::getTagName() const {
    return tagName;
}

const std::vector<Attribute>& ElementNode::getAttributes() const {
    return attributes;
}

const std::vector<std::unique_ptr<BaseNode>>& ElementNode::getChildren() const {
    return children;
}

void ElementNode::addAttribute(const Attribute& attribute) {
    attributes.push_back(attribute);
}

void ElementNode::addChild(std::unique_ptr<BaseNode> child) {
    children.push_back(std::move(child));
}
