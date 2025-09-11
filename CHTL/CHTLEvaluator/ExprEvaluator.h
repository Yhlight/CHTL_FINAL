#pragma once

#include "CHTLNode/ExprNode.h"
#include "CHTLNode/Node.h"
#include <string>
#include <map>
#include <any>

// Forward declare to avoid circular dependency
class VarTemplateDefinitionNode;
namespace Generator {
    using VarTemplateMap = std::map<std::string, VarTemplateDefinitionNode*>;
}


// A simple struct to hold a computed value, which can be a number or a string.
// It also holds the unit if the value is numeric (e.g., "px", "%").
struct EvaluatedValue {
    enum class Type { String, Double } type;
    std::string string_val;
    double double_val = 0.0;
    std::string unit;

    // Helper to convert the value back to a string for CSS output.
    std::string toString() const;
};

// The ExprEvaluator class traverses an expression AST and computes its value.
// It implements the ExprVisitor interface.
class ExprEvaluator : public ExprVisitor {
public:
    // The evaluator needs a context to resolve property references and variables.
    explicit ExprEvaluator(Node& ast_root, std::map<std::string, Generator::VarTemplateMap>& var_templates);

    EvaluatedValue evaluate(ExprNode& expr);

    // Visitor methods
    void visit(LiteralExprNode* node) override;
    void visit(BinaryExprNode* node) override;
    void visit(UnaryExprNode* node) override;
    void visit(TernaryExprNode* node) override;
    void visit(PropertyRefNode* node) override;
    void visit(FunctionCallNode* node) override;

private:
    Node& ast_root;
    std::map<std::string, Generator::VarTemplateMap>& var_templates;
    EvaluatedValue last_value;
    std::map<const PropertyNode*, EvaluatedValue> evaluation_cache;

    PropertyNode* findProperty(const std::string& selector, const std::string& propertyName);
};
