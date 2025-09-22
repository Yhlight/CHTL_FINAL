#include "OriginNode.h"
#include "Visitor.h"

namespace CHTL {

OriginNode::OriginNode(const std::string& content, OriginType type) : content(content), type(type), name("") {}

OriginNode::OriginNode(const std::string& content, OriginType type, const std::string& name) 
    : content(content), type(type), name(name) {}

void OriginNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> OriginNode::clone() const {
    return std::make_unique<OriginNode>(this->content, this->type, this->name);
}

} // namespace CHTL
