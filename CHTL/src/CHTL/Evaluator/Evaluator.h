#pragma once

#include "../ExpressionNode/Expr.h"
#include "Value.h"
#include <string>
#include <map>

namespace CHTL {

// The context for evaluation, mapping variable names (like 'width')
// to their evaluated ChtlValue.
using EvaluationContext = std::map<std::string, ChtlValue>;

// The Evaluator class walks the expression AST and computes a final ChtlValue.
class Evaluator : public ExprVisitor {
public:
    // The main entry point for evaluation.
    ChtlValue evaluate(const Expr* expr, const EvaluationContext& context);

    // Visitor pattern methods
    void visit(const TernaryExpr& expr) override;
    void visit(const BinaryExpr& expr) override;
    void visit(const LiteralExpr& expr) override;
    void visit(const VariableExpr& expr) override;

private:
    ChtlValue m_lastValue;
    const EvaluationContext* m_context = nullptr;

    // Helper to evaluate a sub-expression and return its value
    ChtlValue evaluateSubExpr(const Expr* expr);
};

} // namespace CHTL
