#pragma once

#include "../CHTLNode/Expression/ExprNode.h"
#include "Value.h"
#include <memory>

class Evaluator : public ExprVisitor {
public:
    Value evaluate(std::shared_ptr<ExprNode> expr);

    void visit(BinaryExprNode& expr) override;
    void visit(LiteralExprNode& expr) override;

private:
    Value lastValue;
};
