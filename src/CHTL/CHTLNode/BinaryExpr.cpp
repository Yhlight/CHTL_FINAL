#include "BinaryExpr.h"
#include "ExprVisitor.h"

namespace CHTL {

void BinaryExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
