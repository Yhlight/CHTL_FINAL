#include "ImportNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void ImportNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
