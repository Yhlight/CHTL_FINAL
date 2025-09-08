#pragma once

#include "Expr.h"
#include "../CHTLLexer/Token.h"
#include <memory>

namespace CHTL {

class LiteralExpr : public Expr {
public:
    explicit LiteralExpr(Token literal) : m_literal(std::move(literal)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<LiteralExpr>(m_literal);
    }

    const Token& getLiteral() const { return m_literal; }

private:
    Token m_literal;
};

} // namespace CHTL
