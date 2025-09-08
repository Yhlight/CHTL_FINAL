#pragma once

#include <memory>
#include "../CHTLLexer/Token.h"

namespace CHTL {

class TernaryExpr;
class BinaryExpr;
class LiteralExpr;
class VariableExpr;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(const TernaryExpr& expr) = 0;
    virtual void visit(const BinaryExpr& expr) = 0;
    virtual void visit(const LiteralExpr& expr) = 0;
    virtual void visit(const VariableExpr& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
    virtual std::unique_ptr<Expr> clone() const = 0;
};

} // namespace CHTL
