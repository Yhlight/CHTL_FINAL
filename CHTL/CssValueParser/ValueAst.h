#ifndef CHTL_VALUE_AST_H
#define CHTL_VALUE_AST_H

#include "ValueToken.h"
#include <memory>
#include <string>

// Forward declare nodes and visitor
struct NumericLiteralNode;
struct StringLiteralNode;
struct BinaryOpNode;
struct PropertyReferenceNode;
struct ConditionalExprNode;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(NumericLiteralNode& node) = 0;
    virtual void visit(StringLiteralNode& node) = 0;
    virtual void visit(BinaryOpNode& node) = 0;
    virtual void visit(PropertyReferenceNode& node) = 0;
    virtual void visit(ConditionalExprNode& node) = 0;
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

// Represents a reference to another property (e.g., #box.width)
struct PropertyReferenceNode : ExprNode {
    std::string selector;
    std::string propertyName;
    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
};

// Represents a string literal (e.g., 'red')
struct StringLiteralNode : ExprNode {
    std::string value;
    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
};

// Represents a conditional expression (e.g., condition ? true_expr : false_expr)
struct ConditionalExprNode : ExprNode {
    std::shared_ptr<ExprNode> condition;
    std::shared_ptr<ExprNode> trueBranch;
    std::shared_ptr<ExprNode> falseBranch;
    void accept(ExprVisitor& visitor) override { visitor.visit(*this); }
};

#endif // CHTL_VALUE_AST_H
