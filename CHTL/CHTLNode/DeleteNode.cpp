#include "DeleteNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void DeleteNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
