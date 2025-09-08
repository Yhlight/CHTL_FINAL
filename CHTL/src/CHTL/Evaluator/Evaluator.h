#pragma once

#include "../ExpressionNode/Expr.h"
#include "Value.h"
#include "../CHTLContext/CHTLContext.h"
#include <string>
#include <map>
#include <functional>

namespace CHTL {

class ElementNode; // Forward declare

// The context for evaluation, mapping variable names (like 'width')
// to their evaluated ChtlValue.
using EvaluationContext = std::map<std::string, ChtlValue>;
using NodeResolver = std::function<const ElementNode*(const std::string&)>;

// The Evaluator class walks the expression AST and computes a final ChtlValue.
class Evaluator : public ExprVisitor {
public:
    // The main entry point for evaluation.
    ChtlValue evaluate(const Expr* expr, const CHTLContext& globalContext, const EvaluationContext& localContext, NodeResolver resolver);

    // Visitor pattern methods
    void visit(const TernaryExpr& expr) override;
    void visit(const BinaryExpr& expr) override;
    void visit(const LiteralExpr& expr) override;
    void visit(const VariableExpr& expr) override;
    void visit(const FunctionCallExpr& expr) override;

private:
    ChtlValue m_lastValue;
    const EvaluationContext* m_local_context = nullptr;
    const CHTLContext* m_global_context = nullptr;
    NodeResolver m_node_resolver;

    // Helper to evaluate a sub-expression and return its value
    ChtlValue evaluateSubExpr(const Expr* expr);
};

} // namespace CHTL
