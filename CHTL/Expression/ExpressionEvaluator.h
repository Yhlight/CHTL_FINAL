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

class ExpressionEvaluator : public ExprVisitorString {
public:
    ExpressionEvaluator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates, BaseNode* doc_root);
    std::string evaluate(Expr* expr, ElementNode* context);

    std::string visit(BinaryExpr& expr) override;
    std::string visit(LiteralExpr& expr) override;
    std::string visit(VarExpr& expr) override;
    std::string visit(ReferenceExpr& expr) override;
    std::string visit(ComparisonExpr& expr) override;
    std::string visit(LogicalExpr& expr) override;
    std::string visit(ConditionalExpr& expr) override;
    std::string visit(GroupingExpr& expr) override;


private:
    ElementNode* findElement(BaseNode* context, const std::string& selector);

    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates;
    BaseNode* doc_root;
    ElementNode* current_context = nullptr;
    std::set<std::string> resolution_stack;
};

} // namespace CHTL

#endif // EXPRESSION_EVALUATOR_H
