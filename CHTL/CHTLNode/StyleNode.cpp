#include "StyleNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void StyleNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
