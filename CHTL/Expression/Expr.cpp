#include "Expr.h"

namespace CHTL {

void LiteralExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<Expr> LiteralExpr::clone() const {
    if (is_string) {
        return std::make_unique<LiteralExpr>(string_value);
    }
    return std::make_unique<LiteralExpr>(numeric_value, unit);
}

void BinaryExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<Expr> BinaryExpr::clone() const {
    return std::make_unique<BinaryExpr>(left->clone(), op, right->clone());
}

void VarExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<Expr> VarExpr::clone() const {
    return std::make_unique<VarExpr>(group, name);
}

void ReferenceExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<Expr> ReferenceExpr::clone() const {
    return std::make_unique<ReferenceExpr>(selector, property);
}

void ComparisonExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<Expr> ComparisonExpr::clone() const {
    return std::make_unique<ComparisonExpr>(left->clone(), op, right->clone());
}

void LogicalExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<Expr> LogicalExpr::clone() const {
    return std::make_unique<LogicalExpr>(left->clone(), op, right->clone());
}

void ConditionalExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<Expr> ConditionalExpr::clone() const {
    return std::make_unique<ConditionalExpr>(condition->clone(), then_branch->clone(), else_branch->clone());
}

} // namespace CHTL
