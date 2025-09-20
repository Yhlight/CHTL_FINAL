#include "ReactiveValueNode.h"
#include "../Expression/Expr.h"

namespace CHTL {

void ReactiveValueNode::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<Expr> ReactiveValueNode::clone() const {
    return std::make_unique<ReactiveValueNode>(variable_name);
}

} // namespace CHTL
