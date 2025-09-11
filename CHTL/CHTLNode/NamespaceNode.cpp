#include "NamespaceNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void NamespaceNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
