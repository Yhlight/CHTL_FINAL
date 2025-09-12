#include "NamespaceNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void NamespaceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
