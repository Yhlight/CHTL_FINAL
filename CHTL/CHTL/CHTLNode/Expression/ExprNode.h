#pragma once

#include <memory>
#include "../../CHTLLexer/Token.h"

// Forward declarations for the visitor pattern
class BinaryExprNode;
class LiteralExprNode;

// Visitor for expressions
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(BinaryExprNode& expr) = 0;
    virtual void visit(LiteralExprNode& expr) = 0;
};

// Base class for all expression nodes
class ExprNode {
public:
    virtual ~ExprNode() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};
