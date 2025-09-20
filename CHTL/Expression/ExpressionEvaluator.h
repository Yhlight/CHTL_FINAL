#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include "Expr.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <stdexcept>
#include <cmath>
#include <map>
#include <memory>
#include <set>

namespace CHTL {

class ElementNode;

enum class ValueType { NUMERIC, STRING, BOOL };

struct EvaluatedValue {
    ValueType type = ValueType::NUMERIC;
    double numeric_value = 0.0;
    std::string string_value = "";
};

class ExpressionEvaluator : public ExprVisitor {
public:
    explicit ExpressionEvaluator(const std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>& templates, BaseNode* doc_root);
    EvaluatedValue evaluate(Expr* expr, ElementNode* current_context);

    void visit(BinaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VarExpr& expr) override;
    void visit(ReferenceExpr& expr) override;
    void visit(ComparisonExpr& expr) override;
    void visit(LogicalExpr& expr) override;
    void visit(ConditionalExpr& expr) override;
    void visit(ReactiveValueNode& expr) override;

private:
    ElementNode* findElement(BaseNode* context, const std::string& selector);

    const std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>& templates;
    BaseNode* doc_root;
    ElementNode* current_context = nullptr;
    std::set<std::string> resolution_stack;
    EvaluatedValue result;
};

} // namespace CHTL

#endif // EXPRESSION_EVALUATOR_H
