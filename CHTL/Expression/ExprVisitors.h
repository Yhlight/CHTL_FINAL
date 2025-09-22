#ifndef CHTL_EXPR_VISITORS_H
#define CHTL_EXPR_VISITORS_H

#include "Expr.h"

namespace CHTL {

// A visitor to check if an expression tree contains any reactive nodes.
class ReactivityChecker : public ExprVisitor {
public:
    bool is_reactive = false;

    bool check(Expr* expr) {
        if (expr) {
            expr->accept(*this);
        }
        return is_reactive;
    }

    void visit(BinaryExpr& expr) override {
        expr.left->accept(*this);
        expr.right->accept(*this);
    }
    void visit(LiteralExpr& expr) override {
        // Not reactive
    }
    void visit(VarExpr& expr) override {
        if (expr.override_value) {
            expr.override_value->accept(*this);
        }
    }
    void visit(ReferenceExpr& expr) override {
        // Static references are not reactive in this context
    }
    void visit(ComparisonExpr& expr) override {
        expr.left->accept(*this);
        expr.right->accept(*this);
    }
    void visit(LogicalExpr& expr) override {
        expr.left->accept(*this);
        expr.right->accept(*this);
    }
    void visit(ConditionalExpr& expr) override {
        expr.condition->accept(*this);
        expr.then_branch->accept(*this);
        expr.else_branch->accept(*this);
    }
    void visit(DynamicReferenceExpr& expr) override {
        // This is a form of reactivity
        is_reactive = true;
    }
    void visit(ReactiveVarExpr& expr) override {
        is_reactive = true;
    }
};

} // namespace CHTL

#endif // CHTL_EXPR_VISITORS_H
