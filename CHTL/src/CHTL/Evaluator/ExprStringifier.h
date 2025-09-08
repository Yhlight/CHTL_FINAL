#pragma once

#include "../ExpressionNode/Expr.h"
#include <string>

namespace CHTL {

// A temporary visitor that "stringifies" an expression tree.
// This is used to get the Generator compiling before the full Evaluator is implemented.
// It allows for visual inspection of the parser's output.
class ExprStringifier : public ExprVisitor {
public:
    std::string stringify(const Expr* expr);

    void visit(const TernaryExpr& expr) override;
    void visit(const BinaryExpr& expr) override;
    void visit(const LiteralExpr& expr) override;
    void visit(const VariableExpr& expr) override;

private:
    void parenthesize(const std::string& name, const Expr* expr);
    void parenthesize(const std::string& name, const Expr* left, const Expr* right);

    std::string m_result;
};

} // namespace CHTL
