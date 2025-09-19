#include "Expr.h"

namespace CHTL {

void LiteralExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void BinaryExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void VarExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void ReferenceExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void ComparisonExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void LogicalExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }
void ConditionalExpr::accept(ExprVisitor& visitor) { visitor.visit(*this); }

std::unique_ptr<Expr> LiteralExpr::clone() const {
    return std::make_unique<LiteralExpr>(*this);
}

std::unique_ptr<Expr> BinaryExpr::clone() const {
    return std::make_unique<BinaryExpr>(left->clone(), op, right->clone());
}

std::unique_ptr<Expr> VarExpr::clone() const {
    return std::make_unique<VarExpr>(*this);
}

std::unique_ptr<Expr> ReferenceExpr::clone() const {
    return std::make_unique<ReferenceExpr>(*this);
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

}
