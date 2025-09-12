#include "OriginNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void OriginNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
