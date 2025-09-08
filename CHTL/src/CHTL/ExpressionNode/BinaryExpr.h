#pragma once

#include "Expr.h"
#include "../CHTLLexer/Token.h"
#include <memory>

namespace CHTL {

// Represents a binary operation, e.g., `width > 50px` or `cond1 && cond2`.
// It holds the left and right sub-expressions and the operator token.
class BinaryExpr : public Expr {
public:
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : m_left(std::move(left)), m_operator(std::move(op)), m_right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Expr* getLeft() const { return m_left.get(); }
    const Token& getOperator() const { return m_operator; }
    const Expr* getRight() const { return m_right.get(); }

private:
    std::unique_ptr<Expr> m_left;
    Token m_operator;
    std::unique_ptr<Expr> m_right;
};

} // namespace CHTL
