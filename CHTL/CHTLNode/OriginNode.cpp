#include "OriginNode.h"

namespace CHTL {

void OriginNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> OriginNode::clone() const {
    return std::make_unique<OriginNode>(this->type, this->content);
}

} // namespace CHTL
