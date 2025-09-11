#include "StyleRuleNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void StyleRuleNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
