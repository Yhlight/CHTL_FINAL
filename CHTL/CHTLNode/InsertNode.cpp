#include "InsertNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void InsertNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
