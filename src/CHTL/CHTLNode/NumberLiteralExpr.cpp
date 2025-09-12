#include "NumberLiteralExpr.h"
#include "ExprVisitor.h"

namespace CHTL {

void NumberLiteralExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
