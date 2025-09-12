#ifndef CHTL_VALUE_AST_H
#define CHTL_VALUE_AST_H

#include "ValueToken.h"
#include <memory>
#include <string>

// Forward declare nodes and visitor
struct NumericLiteralNode;
struct BinaryOpNode;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(NumericLiteralNode& node) = 0;
    virtual void visit(BinaryOpNode& node) = 0;
};

// Base class for all expression nodes
struct ExprNode {
    virtual ~ExprNode() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};

// Represents a numeric literal with its unit
struct NumericLiteralNode : ExprNode {
    double value;
    std::string unit;
    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
};

// Represents a binary operation (e.g., 10px + 5px)
struct BinaryOpNode : ExprNode {
    std::shared_ptr<ExprNode> left;
    ValueToken op;
    std::shared_ptr<ExprNode> right;
    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
};

#endif // CHTL_VALUE_AST_H
