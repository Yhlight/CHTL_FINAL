#include "ScriptNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void ScriptNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
