#include "CustomDefinitionNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void CustomDefinitionNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
