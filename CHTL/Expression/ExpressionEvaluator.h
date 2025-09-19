#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include "Expr.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

namespace CHTL {

// Forward declare node types to avoid circular dependencies
class BaseNode;
class ElementNode;

struct EvaluatedValue {
    double value;
    std::string unit;
};

class ExpressionEvaluator : public ExprVisitor {
public:
    ExpressionEvaluator(const std::map<std::string, std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>>& templates, BaseNode* doc_root);
    EvaluatedValue evaluate(Expr* expr, ElementNode* context);

    void visit(BinaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VarExpr& expr) override;
    void visit(ReferenceExpr& expr) override;
    void visit(ComparisonExpr& expr) override;
    void visit(LogicalExpr& expr) override;
    void visit(ConditionalExpr& expr) override;

private:
    ElementNode* findElement(BaseNode* context, const std::string& selector);

    const std::map<std::string, std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>>& templates;
    BaseNode* doc_root;
    ElementNode* current_context = nullptr;
    EvaluatedValue result = {0.0, ""};
    std::set<std::string> resolution_stack;
};

} // namespace CHTL

#endif // EXPRESSION_EVALUATOR_H
