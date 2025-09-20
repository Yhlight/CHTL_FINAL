#include "ImportNode.h"
#include "Visitor.h"

namespace CHTL {

void ImportNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> ImportNode::clone() const {
    // Since this node is handled by the dispatcher and doesn't generate code directly,
    // a shallow clone is sufficient for now. If it needed to be transformed,
    // a deep copy of its members would be required.
    return std::make_unique<ImportNode>(type, path, alias);
}

}
