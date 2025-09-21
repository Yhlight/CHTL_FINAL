#include "OriginUsageNode.h"

namespace CHTL {

void OriginUsageNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> OriginUsageNode::clone() const {
    return std::make_unique<OriginUsageNode>(this->name);
}

} // namespace CHTL
