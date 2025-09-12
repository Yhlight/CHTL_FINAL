#include "ImportNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void ImportNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
