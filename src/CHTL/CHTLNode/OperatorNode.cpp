#include "OperatorNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void OperatorNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
