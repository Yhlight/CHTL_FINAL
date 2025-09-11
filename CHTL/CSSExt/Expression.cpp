#include "Expression.hpp"

namespace CHTL {
namespace CSSExt {

void Binary::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

void Grouping::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

void Literal::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

void Unary::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CSSExt
} // namespace CHTL
