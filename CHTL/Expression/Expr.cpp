#include "Expr.h"

namespace CHTL {

// --- Accept Method Implementations ---
void LiteralExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void BinaryExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void VarExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void ReferenceExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void ComparisonExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void LogicalExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void ConditionalExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }

// --- Clone Method Implementations ---
std::unique_ptr<Expr> LiteralExpr::clone() const {
    return std::make_unique<LiteralExpr>(value, unit);
}
std::unique_ptr<Expr> BinaryExpr::clone() const {
    return std::make_unique<BinaryExpr>(left->clone(), op, right->clone());
}
std::unique_ptr<Expr> VarExpr::clone() const {
    return std::make_unique<VarExpr>(group, name, override_value ? override_value->clone() : nullptr);
}
std::unique_ptr<Expr> ReferenceExpr::clone() const {
    return std::make_unique<ReferenceExpr>(selector, property);
}
std::unique_ptr<Expr> ComparisonExpr::clone() const {
    return std::make_unique<ComparisonExpr>(left->clone(), op, right->clone());
}
std::unique_ptr<Expr> LogicalExpr::clone() const {
    return std::make_unique<LogicalExpr>(left->clone(), op, right->clone());
}
std::unique_ptr<Expr> ConditionalExpr::clone() const {
    return std::make_unique<ConditionalExpr>(condition->clone(), then_branch->clone(), else_branch->clone());
}

void DynamicReferenceExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
std::unique_ptr<Expr> DynamicReferenceExpr::clone() const {
    return std::make_unique<DynamicReferenceExpr>(selector, property);
}

// --- isDynamic Method Implementations ---
bool LiteralExpr::isDynamic() const { return false; }
bool ReferenceExpr::isDynamic() const { return false; }
bool DynamicReferenceExpr::isDynamic() const { return true; }
bool BinaryExpr::isDynamic() const { return left->isDynamic() || right->isDynamic(); }
bool ComparisonExpr::isDynamic() const { return left->isDynamic() || right->isDynamic(); }
bool LogicalExpr::isDynamic() const { return left->isDynamic() || right->isDynamic(); }
bool VarExpr::isDynamic() const { return override_value ? override_value->isDynamic() : false; }
bool ConditionalExpr::isDynamic() const { return condition->isDynamic() || then_branch->isDynamic() || else_branch->isDynamic(); }


} // namespace CHTL
