#pragma once

#include "Node.h"
#include "CHTLLexer/Token.h"
#include <memory>
#include <string>

// Forward declare expression node types
class ExprNode;
class LiteralExprNode;
class BinaryExprNode;
class UnaryExprNode;
class TernaryExprNode;
class PropertyRefNode;

// Visitor for expressions
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(LiteralExprNode* node) = 0;
    virtual void visit(BinaryExprNode* node) = 0;
    virtual void visit(UnaryExprNode* node) = 0;
    virtual void visit(TernaryExprNode* node) = 0;
    virtual void visit(PropertyRefNode* node) = 0;
};

// Base class for all expression nodes
class ExprNode {
public:
    virtual ~ExprNode() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};

using ExprNodePtr = std::unique_ptr<ExprNode>;

// Represents a literal value (e.g., "red", 100px, my-var)
class LiteralExprNode : public ExprNode {
public:
    LiteralExprNode(const Token& value) : value(value) {}
    void accept(ExprVisitor& visitor) override { visitor.visit(this); }
    Token value;
};

// Represents a binary operation (e.g., 100px + 50px)
class BinaryExprNode : public ExprNode {
public:
    BinaryExprNode(ExprNodePtr left, Token op, ExprNodePtr right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
    void accept(ExprVisitor& visitor) override { visitor.visit(this); }
    ExprNodePtr left;
    Token op;
    ExprNodePtr right;
};

// Represents a unary operation (e.g., -10)
class UnaryExprNode : public ExprNode {
public:
    UnaryExprNode(Token op, ExprNodePtr right)
        : op(op), right(std::move(right)) {}
    void accept(ExprVisitor& visitor) override { visitor.visit(this); }
    Token op;
    ExprNodePtr right;
};

// Represents a ternary conditional expression (e.g., width > 50px ? "red" : "blue")
class TernaryExprNode : public ExprNode {
public:
    TernaryExprNode(ExprNodePtr condition, ExprNodePtr trueExpr, ExprNodePtr falseExpr)
        : condition(std::move(condition)), trueExpr(std::move(trueExpr)), falseExpr(std::move(falseExpr)) {}
    void accept(ExprVisitor& visitor) override { visitor.visit(this); }
    ExprNodePtr condition;
    ExprNodePtr trueExpr;
    ExprNodePtr falseExpr;
};

// Represents a reference to another property (e.g., #box.width)
class PropertyRefNode : public ExprNode {
public:
    PropertyRefNode(const Token& selector, const Token& propertyName)
        : selector(selector), propertyName(propertyName) {}
    void accept(ExprVisitor& visitor) override { visitor.visit(this); }
    Token selector;
    Token propertyName;
};
