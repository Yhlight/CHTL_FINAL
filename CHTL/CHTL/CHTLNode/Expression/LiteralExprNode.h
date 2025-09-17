#pragma once

#include "ExprNode.h"

// Represents a literal value in an expression (e.g., "100px", "red")
class LiteralExprNode : public ExprNode {
public:
    LiteralExprNode(Token value) : value(value) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(*this);
    }

    Token value;
};
