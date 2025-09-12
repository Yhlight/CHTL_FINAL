#ifndef CHTL_EVALUATOR_H
#define CHTL_EVALUATOR_H

#include "ValueAst.h"
#include "../CHTLNode/BaseNode.h"
#include <memory>
#include <string>

// A simple struct to hold the result of an evaluation.
struct EvaluatedValue {
    double value;
    std::string unit;
    bool hasError = false;
    std::string errorMessage;
};

class Evaluator : public ExprVisitor {
public:
    EvaluatedValue evaluate(std::shared_ptr<ExprNode> expr, std::shared_ptr<BaseNode> ChtlAstRoot);

    // Visitor methods
    void visit(NumericLiteralNode& node) override;
    void visit(BinaryOpNode& node) override;
    void visit(PropertyReferenceNode& node) override;

private:
    EvaluatedValue result;
    std::shared_ptr<BaseNode> chtlAstRoot; // Store the root for querying
};

#endif // CHTL_EVALUATOR_H
