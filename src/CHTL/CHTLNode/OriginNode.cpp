#include "OriginNode.h"
#include "Visitor.h"

OriginNode::OriginNode(const std::string& type, const std::string& content)
    : type(type), content(content) {}

void OriginNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

const std::string& OriginNode::getType() const {
    return type;
}

const std::string& OriginNode::getContent() const {
    return content;
}
