#include "OriginNode.h"
#include "Visitor.h"

namespace CHTL {

void OriginNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> OriginNode::clone() const {
    return std::make_unique<OriginNode>(this->content, this->type, this->name);
}

} // namespace CHTL
