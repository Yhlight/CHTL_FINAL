#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include "Expr.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <vector>
#include <map>
#include <set>

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
    ExpressionEvaluator(
        const std::map<std::string, AttributeNode>* style_context,
        const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates,
        BaseNode* doc_root
    );
    EvaluatedValue evaluate(Expr* expr, ElementNode* context_element);

    void visit(BinaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VarExpr& expr) override;
    void visit(ReferenceExpr& expr) override;
    void visit(ComparisonExpr& expr) override;
    void visit(LogicalExpr& expr) override;
    void visit(ConditionalExpr& expr) override;
    void visit(BoolExpr& expr) override;

private:
    ElementNode* findElement(BaseNode* context, const std::string& selector);
    EvaluatedValue getPropertyValue(ElementNode* element, const std::string& prop_name);

    const std::map<std::string, AttributeNode>* style_context;
    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates;
    BaseNode* doc_root;
    ElementNode* current_element_context = nullptr;
    EvaluatedValue result = {0.0, ""};
    std::set<std::string> resolution_stack;
};

} // namespace CHTL

#endif // EXPRESSION_EVALUATOR_H
