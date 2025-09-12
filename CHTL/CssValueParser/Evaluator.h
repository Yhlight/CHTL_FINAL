#ifndef CHTL_EVALUATOR_H
#define CHTL_EVALUATOR_H

#include "ValueAst.h"
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
    EvaluatedValue evaluate(std::shared_ptr<ExprNode> expr);

    // Visitor methods
    void visit(NumericLiteralNode& node) override;
    void visit(BinaryOpNode& node) override;

private:
    EvaluatedValue result;
};

#endif // CHTL_EVALUATOR_H
