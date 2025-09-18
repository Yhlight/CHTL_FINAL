#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include "Expr.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <stdexcept>
#include <cmath> // For pow()
#include <map>
#include <set>

namespace CHTL {

// A struct to hold the result of an evaluation.
struct EvaluatedValue {
    double value = 0.0;
    std::string unit = "";
};

// This class implements the ExprVisitor pattern to calculate the result of an expression tree.
class ExpressionEvaluator : public ExprVisitor {
public:
    explicit ExpressionEvaluator(const std::map<std::string, TemplateDefinitionNode>& templates);
    EvaluatedValue evaluate(Expr* expr);

    void visit(BinaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VarExpr& expr) override;

private:
    const std::map<std::string, TemplateDefinitionNode>& templates;
    std::set<std::string> resolution_stack; // For circular dependency detection
    EvaluatedValue result;
};

} // namespace CHTL

#endif // EXPRESSION_EVALUATOR_H
