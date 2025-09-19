#include "OriginNode.h"
#include "Visitor.h"

namespace CHTL {

OriginNode::OriginNode(const std::string& content, OriginType type) : content(content), type(type) {}

void OriginNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> OriginNode::clone() const {
    return std::make_unique<OriginNode>(this->content, this->type);
}

} // namespace CHTL
