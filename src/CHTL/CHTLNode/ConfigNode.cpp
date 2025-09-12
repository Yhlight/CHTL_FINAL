#include "ConfigNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void ConfigNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
