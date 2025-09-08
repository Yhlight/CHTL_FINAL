#pragma once

#include "Expr.h"
#include <memory>

namespace CHTL {

// Represents a ternary conditional operation, e.g., `condition ? then : else`.
// It holds three sub-expressions for the condition, the 'then' branch,
// and the 'else' branch.
class TernaryExpr : public Expr {
public:
    TernaryExpr(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> thenBranch, std::unique_ptr<Expr> elseBranch)
        : m_condition(std::move(condition)), m_thenBranch(std::move(thenBranch)), m_elseBranch(std::move(elseBranch)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Expr* getCondition() const { return m_condition.get(); }
    const Expr* getThenBranch() const { return m_thenBranch.get(); }
    const Expr* getElseBranch() const { return m_elseBranch.get(); }

private:
    std::unique_ptr<Expr> m_condition;
    std::unique_ptr<Expr> m_thenBranch;
    std::unique_ptr<Expr> m_elseBranch;
};

} // namespace CHTL
