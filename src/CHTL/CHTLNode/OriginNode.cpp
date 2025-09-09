#include "OriginNode.h"
#include "Visitor.h"

OriginNode::OriginNode(const std::string& type, const std::string& content)
    : type(type), content(content) {}

void OriginNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> OriginNode::clone() const {
    return std::make_unique<OriginNode>(type, content);
}

const std::string& OriginNode::getType() const {
    return type;
}

const std::string& OriginNode::getContent() const {
    return content;
}
