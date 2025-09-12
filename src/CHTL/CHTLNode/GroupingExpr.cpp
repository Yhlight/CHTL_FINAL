#include "GroupingExpr.h"
#include "ExprVisitor.h"

namespace CHTL {

void GroupingExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
