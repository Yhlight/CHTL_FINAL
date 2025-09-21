#include "OriginNode.h"
#include "Visitor.h"

namespace CHTL {

OriginNode::OriginNode(std::string type, std::string content, std::string name)
    : type(std::move(type)), content(std::move(content)), name(std::move(name)) {}

void OriginNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> OriginNode::clone() const {
    return std::make_unique<OriginNode>(this->type, this->content, this->name);
}

} // namespace CHTL
