#include "CustomNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void CustomNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
