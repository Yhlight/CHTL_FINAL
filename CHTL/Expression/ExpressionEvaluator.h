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

struct PropertyValue {
    bool is_string = false;
    double numeric_value = 0.0;
    std::string unit;
    std::string string_value;

    std::string toString() const {
        if (is_string) {
            return string_value;
        }
        // Simplified to_string, good enough for CSS values.
        std::string num_str = std::to_string(numeric_value);
        // Remove trailing zeros
        num_str.erase(num_str.find_last_not_of('0') + 1, std::string::npos);
        if (num_str.back() == '.') {
            num_str.pop_back();
        }
        return num_str + unit;
    }
};

class ExpressionEvaluator : public ExprVisitor {
public:
    ExpressionEvaluator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates, BaseNode* doc_root);
    PropertyValue evaluate(Expr* expr, ElementNode* context);

    void visit(BinaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VarExpr& expr) override;
    void visit(ReferenceExpr& expr) override;
    void visit(ComparisonExpr& expr) override;
    void visit(LogicalExpr& expr) override;
    void visit(ConditionalExpr& expr) override;

private:
    ElementNode* findElement(BaseNode* context, const std::string& selector);

    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates;
    BaseNode* doc_root;
    ElementNode* current_context = nullptr;
    PropertyValue result;
    std::set<std::string> resolution_stack;
};

} // namespace CHTL

#endif // EXPRESSION_EVALUATOR_H
