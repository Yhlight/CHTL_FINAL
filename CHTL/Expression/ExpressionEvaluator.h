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

class ElementNode; // Forward declaration

// A struct to hold the result of an evaluation.
enum class ValueType { NUMERIC, STRING, BOOL };

struct EvaluatedValue {
    ValueType type = ValueType::NUMERIC;
    double numeric_value = 0.0;
    std::string string_value = ""; // For units with numbers, or full string values
};

// This class implements the ExprVisitor pattern to calculate the result of an expression tree.
class ExpressionEvaluator : public ExprVisitor {
public:
    explicit ExpressionEvaluator(const std::map<std::string, TemplateDefinitionNode>& templates, BaseNode* doc_root);
    EvaluatedValue evaluate(Expr* expr, ElementNode* current_context);

    void visit(BinaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VarExpr& expr) override;
    void visit(ReferenceExpr& expr) override;
    void visit(ComparisonExpr& expr) override;
    void visit(LogicalExpr& expr) override;
    void visit(ConditionalExpr& expr) override;

private:
    ElementNode* findElement(BaseNode* context, const std::string& selector);

    const std::map<std::string, TemplateDefinitionNode>& templates;
    BaseNode* doc_root;
    ElementNode* current_context = nullptr;
    std::set<std::string> resolution_stack; // For circular dependency detection
    EvaluatedValue result;
};

} // namespace CHTL

#endif // EXPRESSION_EVALUATOR_H
