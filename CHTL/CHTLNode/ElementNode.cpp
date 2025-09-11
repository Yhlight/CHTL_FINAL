#include "ElementNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void ElementNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
