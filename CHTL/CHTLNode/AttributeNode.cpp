#include "AttributeNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void AttributeNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
