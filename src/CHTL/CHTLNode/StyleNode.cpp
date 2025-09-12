#include "StyleNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void StyleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
