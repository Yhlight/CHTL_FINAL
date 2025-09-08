#pragma once

#include "Expr.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

// Represents a literal value in an expression, such as a number, string, or
// an unquoted literal like '50px' or 'red'.
class LiteralExpr : public Expr {
public:
    explicit LiteralExpr(Token literal) : m_literal(std::move(literal)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token& getLiteral() const { return m_literal; }

private:
    Token m_literal;
};

} // namespace CHTL
