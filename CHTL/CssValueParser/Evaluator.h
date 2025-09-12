#ifndef CHTL_EVALUATOR_H
#define CHTL_EVALUATOR_H

#include "ValueAst.h"
#include "../CHTLNode/BaseNode.h"
#include <memory>
#include <string>
#include <variant>
#include <set>

// A struct to hold a numeric value and its unit.
struct NumberWithUnit {
    double value;
    std::string unit;
};

// A variant to hold the different types of values our expressions can produce.
using EvalResultVariant = std::variant<NumberWithUnit, bool, std::string>;

// A struct to hold the full result of an evaluation, including errors.
struct EvaluatedValue {
    EvalResultVariant value;
    bool hasError = false;
    std::string errorMessage;
};

class Evaluator : public ExprVisitor {
public:
    EvaluatedValue evaluate(
        std::shared_ptr<ExprNode> expr,
        std::shared_ptr<BaseNode> chtlAstRoot,
        class ElementNode* currentElement,
        std::set<std::string>& evaluationStack
    );

    // Visitor methods
    void visit(NumericLiteralNode& node) override;
    void visit(StringLiteralNode& node) override;
    void visit(BinaryOpNode& node) override;
    void visit(PropertyReferenceNode& node) override;
    void visit(ConditionalExprNode& node) override;

private:
    EvaluatedValue result;
    std::shared_ptr<BaseNode> chtlAstRoot;
    ElementNode* currentElement = nullptr;
    std::set<std::string>* evaluationStack = nullptr;
};

#endif // CHTL_EVALUATOR_H
