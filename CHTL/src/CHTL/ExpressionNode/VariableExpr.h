#pragma once

#include "Expr.h"
#include "../CHTLLexer/Token.h"
#include <memory>

namespace CHTL {

class VariableExpr : public Expr {
public:
    explicit VariableExpr(Token name) : m_name(std::move(name)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<VariableExpr>(m_name);
    }

    const Token& getName() const { return m_name; }

private:
    Token m_name;
};

} // namespace CHTL
