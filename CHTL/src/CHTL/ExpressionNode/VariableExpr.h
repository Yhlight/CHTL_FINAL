#pragma once

#include "Expr.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

// Represents a variable name in an expression, such as `width`.
// The evaluator will use this to look up the value of another property
// on the same element.
class VariableExpr : public Expr {
public:
    explicit VariableExpr(Token name) : m_name(std::move(name)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token& getName() const { return m_name; }

private:
    Token m_name;
};

} // namespace CHTL
