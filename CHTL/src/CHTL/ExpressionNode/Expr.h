#pragma once

#include <memory>
#include "../CHTLLexer/Token.h"

namespace CHTL {

// Forward declarations for all expression node types
class TernaryExpr;
class BinaryExpr;
class LiteralExpr;
class VariableExpr;

// The Visitor interface for the expression AST.
// This design pattern allows for clean separation of operations (like evaluation)
// from the node classes themselves.
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    // The specific visit methods will be implemented by concrete visitors (e.g., the Evaluator).
    // Note: The return type could be a generic value type in a real implementation,
    // but for now, we can handle return values within the visitor's state.
    virtual void visit(const TernaryExpr& expr) = 0;
    virtual void visit(const BinaryExpr& expr) = 0;
    virtual void visit(const LiteralExpr& expr) = 0;
    virtual void visit(const VariableExpr& expr) = 0;
};

// The abstract base class for all expression nodes.
class Expr {
public:
    virtual ~Expr() = default;
    // The accept method is the entry point for the visitor pattern.
    virtual void accept(ExprVisitor& visitor) const = 0;
};

} // namespace CHTL
