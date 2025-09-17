#pragma once

#include "ExprNode.h"

// Represents a binary operation (e.g., left + right)
class BinaryExprNode : public ExprNode {
public:
    BinaryExprNode(std::shared_ptr<ExprNode> left, Token op, std::shared_ptr<ExprNode> right)
        : left(left), op(op), right(right) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::shared_ptr<ExprNode> left;
    Token op;
    std::shared_ptr<ExprNode> right;
};
